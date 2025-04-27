#pragma once

#include "../ecs/world.hpp"
#include "../components/repeated-controller.hpp"
#include "../components/camera.hpp"
#include "../application.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our {

    class RepeatControllerSystem {
        Application* app;

    public:
        void enter(Application* app) {
            this->app = app;
        }

        void update(World* world, float deltaTime) {
            CameraComponent* camera = nullptr;

            for (auto entity : world->getEntities()) {
                camera = entity->getComponent<CameraComponent>();
                if (camera) break;
            }
            if (!camera) return;

            Entity* camEntity = camera->getOwner();
            glm::vec3& cam_position = camEntity->localTransform.position;

            for (auto entity : world->getEntities()) {
                auto controller = entity->getComponent<RepeatControllerComponent>();
                if (!controller) continue;

                glm::vec3& position = entity->localTransform.position;

                // Move towards player
                position.z += controller->speed * deltaTime;

                // Floating effect
                position.y += glm::sin(controller->currentTime * controller->floatSpeed) * controller->floatAmplitude * deltaTime;

                controller->currentTime += deltaTime;

                // When passes camera
                if (position.z > cam_position.z + 5.0f) {
                    position.z = cam_position.z - 40.0f; // Restart behind

                    if (controller->entityType == "ghost") {
                        position.x = cam_position.x + (rand() % 20 - 10); // Ghosts float randomly
                        position.y = cam_position.y + (rand() % 5);
                    } else if (controller->entityType == "heart") {
                        position.x = cam_position.x + (rand() % 10 - 5); // Hearts spawn closer
                        position.y = cam_position.y + (rand() % 2 + 1); // Hearts not too high
                    }

                    controller->currentTime = 0.0f;
                    entity->hidden = false;
                }
            }
        }

        void exit() {}
    };

}
