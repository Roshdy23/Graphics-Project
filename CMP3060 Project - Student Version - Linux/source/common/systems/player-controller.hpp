#pragma once

#include "../ecs/world.hpp"
#include "../components/player-controller.hpp"
#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace our
{

    class PlayerControllerSystem
    {
        Application *app;
        Entity *playerEntity;
        Entity *cameraEntity;

        PlayerControllerComponent *controller;

        bool mouse_locked = false;

        bool isRightKeyPressed = false;
        bool isLeftKeyPressed = false;
        bool isHovering = false;
        bool isPhasing = false;
        float phaseTimer = 0.0f;
        float maxPhaseDuration = 2.0f; // seconds
        glm::vec3 targetLanePosition;

    public:
        void enter(Application *app)
        {
            this->app = app;
        }

        void setPlayer(Entity *player)
        {
            this->playerEntity = player;
            this->controller = playerEntity->getComponent<PlayerControllerComponent>();
            targetLanePosition = playerEntity->localTransform.position;
        }

        Entity *getPlayer()
        {
            return playerEntity;
        }

        void setCamera(Entity *camera)
        {
            this->cameraEntity = camera;
        }

        // void update(World *world, float deltaTime)
        // {
        //     if (!playerEntity) return;

        //     controller = playerEntity->getComponent<PlayerControllerComponent>();
        //     glm::vec3 &position = playerEntity->localTransform.position;

        //     // Matrix for direction vectors
        //     glm::mat4 matrix = playerEntity->localTransform.toMat4();
        //     glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0));
        //     glm::vec3 right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

        //     // --- Auto Forward Movement (Twilight Drift)
        //     float driftSpeed = controller->moveSpeed;
        //     position -= front * (driftSpeed * deltaTime);

        //     // --- Smooth Lane Switching (Floating)
        //     if (app->getKeyboard().isPressed(GLFW_KEY_RIGHT))
        //     {
        //         if (!isRightKeyPressed)
        //         {
        //             targetLanePosition.x -= 2.0f; // Move right
        //             targetLanePosition.x = glm::clamp(targetLanePosition.x, -2.0f, 2.0f);
        //             isRightKeyPressed = true;
        //         }
        //     }
        //     else isRightKeyPressed = false;

        //     if (app->getKeyboard().isPressed(GLFW_KEY_LEFT))
        //     {
        //         if (!isLeftKeyPressed)
        //         {
        //             targetLanePosition.x += 2.0f; // Move left
        //             targetLanePosition.x = glm::clamp(targetLanePosition.x, -2.0f, 2.0f);
        //             isLeftKeyPressed = true;
        //         }
        //     }
        //     else isLeftKeyPressed = false;

        //     // Smoothly glide toward target lane position
        //     position.x = glm::mix(position.x, targetLanePosition.x, 5.0f * deltaTime);

        //     // --- Hover Mechanism (Gentle Jump Up)
        //     if (app->getKeyboard().isPressed(GLFW_KEY_UP))
        //     {
        //         if (!isHovering && controller->canJump)
        //         {
        //             controller->velocity.y = controller->jumpForce;
        //             isHovering = true;
        //             controller->canJump = false;
        //         }
        //     }

        //     if (isHovering)
        //     {
        //         position.y += controller->velocity.y * deltaTime;
        //         controller->velocity.y -= controller->gravity * deltaTime;

        //         if (position.y <= 1.0f)
        //         {
        //             position.y = 1.0f;
        //             isHovering = false;
        //             controller->velocity.y = 0.0f;
        //             controller->canJump = true;
        //         }
        //     }

        //     // --- Phase (Slide through obstacles)
        //     if (app->getKeyboard().justPressed(GLFW_KEY_DOWN))
        //     {
        //         if (!isPhasing)
        //         {
        //             isPhasing = true;
        //             phaseTimer = 0.0f;
        //             // Here you can also enable ghost visual if you want
        //         }
        //     }

        //     if (isPhasing)
        //     {
        //         phaseTimer += deltaTime;
        //         if (phaseTimer >= maxPhaseDuration)
        //         {
        //             isPhasing = false;
        //             // Disable ghost visual here if needed
        //         }
        //     }

        //     // --- Update Camera
        //     if (cameraEntity)
        //     {
        //         cameraEntity->localTransform.position.x = position.x;
        //         cameraEntity->localTransform.position.z = position.z + 5.0f; // camera stays slightly behind
        //     }
        // }
        void update(World *world, float deltaTime)
        {
            if (!playerEntity)
                return;

            controller = playerEntity->getComponent<PlayerControllerComponent>();
            glm::vec3 &position = playerEntity->localTransform.position;

            // Matrix for direction vectors
            glm::mat4 matrix = playerEntity->localTransform.toMat4();
            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -0.5, 0)); // Forward direction
            glm::vec3 right = glm::vec3(matrix * glm::vec4(-0.5, 0, 0, 0)); // Right direction

            // --- Forward Movement (Up Arrow Key)
            if (app->getKeyboard().isPressed(GLFW_KEY_UP))
            {
                position -= front * (controller->moveSpeed * deltaTime); // Move forward
            }

            // --- Lateral Movement (Left and Right Arrow Keys)
            if (app->getKeyboard().isPressed(GLFW_KEY_RIGHT))
            {
                position += right * (controller->moveSpeed * deltaTime); // Move right
            }
            else if (app->getKeyboard().isPressed(GLFW_KEY_LEFT))
            {
                position -= right * (controller->moveSpeed * deltaTime); // Move left
            }

            // --- Update Camera
            if (cameraEntity)
            {
                // Make the camera follow the player
                glm::vec3 newCameraPosition = position;

                // Position camera slightly behind the player along Z-axis
                newCameraPosition.z = position.z + 0.5f;

                // Adjust the camera's height (slightly above the player for a better view)
                newCameraPosition.y = position.y + 2.5f;

                // Update camera's position
                cameraEntity->localTransform.position = newCameraPosition;
                        // Print the Z positions
        // std::cout << "Player Z Position: " << position.z << std::endl;
        // std::cout << "Camera Z Position: " << newCameraPosition.z << std::endl;
            }
        }

        void exit()
        {
            if (mouse_locked)
            {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }
    };

}
