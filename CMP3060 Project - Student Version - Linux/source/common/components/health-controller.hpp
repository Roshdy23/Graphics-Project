#pragma once

#include "../ecs/component.hpp"

namespace our {

    class HealthComponent : public Component {
    public:
        float health = 100.0f;  // Health value
        bool isAlive = true;    // Whether the entity is alive or dead

        static std::string getID() { return "Health"; }

        void deserialize(const nlohmann::json& data) override;
    };

}
