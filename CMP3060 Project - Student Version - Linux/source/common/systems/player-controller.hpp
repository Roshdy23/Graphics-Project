#pragma once

#include "../ecs/world.hpp"
#include "../components/player-controller.hpp"
#include "../components/movement.hpp"
#include "../application.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace our {

class PlayerControllerSystem {
    Application* app;
    Entity* player;

public:
    void setPlayer(Entity* playerEntity) {
        this->player = playerEntity;
    }

    void setApplication(Application* application) {
        this->app = application;
    }

    void update(float deltaTime) {
        if (!player) return;

        auto controller = player->getComponent<PlayerControllerComponent>();
        if (!controller) return;

        glm::vec3 direction = {0, 0, 0};

        // Handle input
        if (app->getKeyboard().isPressed(GLFW_KEY_W)) direction.z -= 1;
        if (app->getKeyboard().isPressed(GLFW_KEY_S)) direction.z += 1;
        if (app->getKeyboard().isPressed(GLFW_KEY_A)) direction.x -= 1;
        if (app->getKeyboard().isPressed(GLFW_KEY_D)) direction.x += 1;

        // Normalize direction vector
        if (glm::length(direction) > 0.0f) direction = glm::normalize(direction);

        // Apply running multiplier
        float speed = controller->moveSpeed;
        if (app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT))
            speed *= controller->runMultiplier;

        // Apply horizontal movement
        controller->velocity.x = direction.x * speed;
        controller->velocity.z = direction.z * speed;

        // Gravity
        controller->velocity.y -= controller->gravity * deltaTime;

        // Jumping
        if (app->getKeyboard().justPressed(GLFW_KEY_SPACE) && controller->canJump) {
            controller->velocity.y = controller->jumpForce;
            controller->canJump = false;
        }

        // Update player position
        player->localTransform.position += controller->velocity * deltaTime;

        // Simple ground check (y=0 is ground)
        if (player->localTransform.position.y <= 0.0f) {
            player->localTransform.position.y = 0.0f;
            controller->velocity.y = 0.0f;
            controller->canJump = true;
        }
    }
};

}
