#pragma once

#include "../ecs/component.hpp"
#include <string>

namespace our {

    class RepeatControllerComponent : public Component {
    public:
    float speed = 5.0f;
    std::string entityType = "ghost";

    // Track previous camera position to detect forward movement
    float lastCameraZ = -9999.0f;
        // std::string entityType = "ghost"; // Can be "ghost" or "heart"
        float currentTime = 0.0f;
        float initialOffsetZ = 0.0f;   // Distance from camera at the start
        float originalY = 0.0f;        // Fixed Y value to avoid floating
        bool initialized = false;      // To store initial state only once

        static std::string getID() { return "Repeat Controller"; }

        void deserialize(const nlohmann::json& data) override;
    };

}