#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include <iostream>
#include "../components/health-controller.hpp"
#include <GLFW/glfw3.h>

namespace our
{
    bool lit = false;
    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config, Entity *player)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;
        this->player = player;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;

            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;

            skyPipelineState.setup();

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer
            glGenFramebuffers(1, &postprocessFrameBuffer);

            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).
            colorTarget = new Texture2D();
            colorTarget->bind();

            GLsizei levelsCnt = (GLsizei)glm::floor(glm::log2((float)glm::max(windowSize.x, windowSize.y))) + 1;
            glTexStorage2D(GL_TEXTURE_2D, levelsCnt, GL_RGBA8, windowSize.x, windowSize.y);

            depthTarget = new Texture2D();
            depthTarget->bind();

            glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, windowSize.x, windowSize.y);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            // TODO: (Req 11) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
        lights = {};
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        glm::vec3 playerPosition = player->localTransform.position;

        float playerStealthLevel = 1.0f;

        for (auto entity : world->getEntities())
        {
            if (entity->hidden)
                continue;
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();

            if(entity->name == "player"){
                playerStealthLevel = entity->getComponent<HealthComponent>()->health;
                playerStealthLevel = (playerStealthLevel + 20 <= 100) ? playerStealthLevel += 20 : playerStealthLevel;
            }

            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
            if (auto light = player->getComponent<LightComponent>(); light)
            {
                if (light)
                {
                    if (light->lightType == 2)
                    {
                        // printf("light type 2\n");
                        light->position = playerPosition;
                        light->position.y += 1.5;
                        if (playerPosition.x < -1)
                        {
                            light->position.x -= 2;
                        }
                        else if (playerPosition.x > 1)
                        {
                            light->position.x += 2;
                        }
                    }
                    else if (light->lightType == 1)
                        light->position.z = playerPosition.z + light->displacement;

                    // std::cout << "light x:" << light->position.x << " y:" << light->position.y << " z:" << light->position.z << std::endl;
                    // std::cout << "player x:" << playerPosition.x << " y:" << playerPosition.y << " z:" << playerPosition.z << std::endl;
                    // std::cout << "\n";
                    lights.push_back(light);
                }
            }
        }
        lit = true;
        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        auto cameraMatrix = camera->getOwner()->getLocalToWorldMatrix();
        glm::vec3 cameraForward = glm::normalize(glm::vec3(cameraMatrix[2]));
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
            //TODO: (Req 9) Finish this function
            // HINT: the following return should return true "first" should be drawn before "second". 
            float distance1 = glm::dot(first.center, cameraForward);
            float distance2 = glm::dot(second.center, cameraForward);
            return distance1 < distance2; });

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();
        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize
        glm::vec2 viewportStart = glm::vec2(0, 0);
        glm::vec2 viewportSize = windowSize;

        // Set the OpenGL viewport
        glViewport(viewportStart.x, viewportStart.y, viewportSize.x, viewportSize.y);

        // TODO: (Req 9) Set the clear color to black and the clear depth to 1
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);

        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        // TODO: (Req 9) Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto &command : opaqueCommands)
        {
            glm::mat4 MVP = VP * command.localToWorld;
            command.material->setup();
            command.material->shader->set("transform", MVP);
            command.mesh->draw();
            if (auto material_light = dynamic_cast<LightingMaterial *>(command.material); material_light)
            {
                material_light->shader->set("sky.top", glm::vec3(0.0f, 0.1f, 0.5f));
                material_light->shader->set("sky.horizon", glm::vec3(0.3f, 0.3f, 0.3f));
                material_light->shader->set("sky.bottom", glm::vec3(0.1f, 0.1f, 0.1f));

                material_light->shader->set("light_count", int(lights.size()));

                material_light->shader->set("VP", VP);
                material_light->shader->set("M_IT", glm::transpose(glm::inverse(command.localToWorld)));
                glm::vec3 cameraPosition = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);

                material_light->shader->set("camera_position", cameraPosition);
                material_light->shader->set("M", command.localToWorld);

                for (int i = 0; i < lights.size(); i++)
                {
                    material_light->shader->set("lights[" + std::to_string(i) + "].position", lights[i]->position);
                    material_light->shader->set("lights[" + std::to_string(i) + "].type", lights[i]->lightType);
                    material_light->shader->set("lights[" + std::to_string(i) + "].direction", lights[i]->direction);
                    material_light->shader->set("lights[" + std::to_string(i) + "].color", lights[i]->color);
                    material_light->shader->set("lights[" + std::to_string(i) + "].attenuation", lights[i]->attenuation);
                    material_light->shader->set("lights[" + std::to_string(i) + "].cone_angles", lights[i]->cone_angles);
                }
            }
        }

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 10) setup the sky material
            this->skyMaterial->setup();

            // TODO: (Req 10) Get the camera position
            glm::vec3 cameraPosition = camera->getOwner()->localTransform.position;

            // TODO: (Req 10) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            our::Transform skyTransformer;
            skyTransformer.position = cameraPosition;
            glm::mat4 skyModel = skyTransformer.toMat4();

            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f);

            // TODO: (Req 10) set the "transform" uniform
            glm::mat4 finalTransform = alwaysBehindTransform * VP * skyModel;
            this->skyMaterial->shader->set("transform", finalTransform);

            // TODO: (Req 10) draw the sky sphere
            this->skySphere->draw();
        }
        // TODO: (Req 9) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto &command : transparentCommands)
        {
            glm::mat4 MVP = VP * command.localToWorld;
            command.material->setup();
            command.material->shader->set("transform", MVP);
            command.mesh->draw();
        }

        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial)
        {
            // TODO: (Req 11) Return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle
            postprocessMaterial->setup();
            postprocessMaterial->shader->set("stealthLevel", playerStealthLevel / 100.0f);
            postprocessMaterial->shader->set("time", (float)glfwGetTime());
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
}