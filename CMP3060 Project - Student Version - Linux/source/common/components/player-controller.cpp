#include "player-controller.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {

    // Reads moveSpeed, runMultiplier, gravity, and jumpForce from the given JSON data
    void PlayerControllerComponent::deserialize(const nlohmann::json& data) {
        if (!data.is_object()) return;
        moveSpeed = data.value("moveSpeed", moveSpeed);
        runMultiplier = data.value("runMultiplier", runMultiplier);
        gravity = data.value("gravity", gravity);
        jumpForce = data.value("jumpForce", jumpForce);
    }

}
