#pragma once

#include "../ecs/component.hpp"
#include <string>

namespace our {

    class RepeatControllerComponent : public Component {
    public:
        float speed = 3.0f; // Speed towards player
        float floatAmplitude = 0.5f; // How much it floats up/down
        float floatSpeed = 2.0f; // How fast it floats
        std::string entityType = "ghost"; // Can be "ghost" or "heart"
        float currentTime = 0.0f;

        static std::string getID() { return "Repeat Controller"; }

        void deserialize(const nlohmann::json& data) override;
    };

}