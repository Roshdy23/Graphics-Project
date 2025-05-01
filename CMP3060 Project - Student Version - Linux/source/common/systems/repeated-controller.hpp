#pragma once

#include "../ecs/world.hpp"
#include "../components/repeated-controller.hpp"
#include <application.hpp>
#include <glm/glm.hpp>
#include <cstdlib>

namespace our {

    class RepeatControllerSystem {
        Application* app = nullptr;
        Entity* cameraEntity = nullptr;

    public:
        void enter(Application* app) {
            this->app = app;
        }

        void setCamera(Entity* camera) {
            this->cameraEntity = camera;
        }

        void update(World* world, float deltaTime) {
            if (!cameraEntity) return;

            glm::vec3& cam_position = cameraEntity->localTransform.position;

            for (auto entity : world->getEntities()) {
                auto controller = entity->getComponent<RepeatControllerComponent>();
                if (!controller) continue;

                glm::vec3& position = entity->localTransform.position;

                // On first update, store initial Z offset between object and camera
                if (!controller->initialized) {
                    controller->initialOffsetZ = position.z - cam_position.z;
                    controller->originalY = position.y; // Store fixed Y
                    controller->initialized = true;
                }

                // Move forward (simulate world scrolling backward)
                position.z += controller->speed * deltaTime;

                // Repeat object when it goes behind camera
                if (position.z > cam_position.z + 5.0f) {
                    // Place at same relative distance as at start
                    position.z = cam_position.z + controller->initialOffsetZ;

                    // Adjust X randomly but keep Y fixed
                    if (controller->entityType == "ghost") {
                        position.x = cam_position.x + (rand() % 21 - 10); // [-10, 10]
                    } else if (controller->entityType == "heart") {
                        position.x = cam_position.x + (rand() % 11 - 5);  // [-5, 5]
                    }

                    position.y = controller->originalY; // no floating, fixed height

                    controller->currentTime = 0.0f;
                    entity->hidden = false;
                }
            }
        }

        void exit() {}
    };

}
