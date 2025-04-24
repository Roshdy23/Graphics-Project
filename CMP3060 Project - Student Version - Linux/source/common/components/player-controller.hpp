#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>

namespace our {

    // This component controls the movement of the player in the game.
    // It handles player speed, jumping, gravity, and direction.
    class PlayerControllerComponent : public Component {
    public:
        glm::vec3 velocity = {0, 0, 0}; // Player's current velocity
        float moveSpeed = 4.0f;         // Walking speed
        float runMultiplier = 1.5f;     // Speed multiplier when running (shift key)
        float gravity = 9.8f;           // Gravity constant (downward acceleration)
        float jumpForce = 5.0f;         // Jump force (how high the player can jump)
        bool canJump = false;           // Flag to check if the player can jump
        glm::vec3 jumpDirection = {0.0f, 1.0f, 0.0f}; // Direction of jump (usually y-axis)

        // Returns the ID of the component (useful for serialization)
        static std::string getID() { return "PlayerController"; }

        // Deserialize movement and jump parameters from a JSON object
        void deserialize(const nlohmann::json& data) override;
    };

}
