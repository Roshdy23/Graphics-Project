#include "health-controller.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {

    void HealthComponent::deserialize(const nlohmann::json& data) {
        if (!data.is_object()) return;
        health = data.value("health", health);
        isAlive = data.value("isAlive", isAlive);
    }

}
