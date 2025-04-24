#pragma once

#include "../ecs/component.hpp"
#include <string>

namespace our {

    // The KeyComponent will hold data about the key: type and collection status.
    class KeyComponent : public Component {
    public:
        std::string keyType;    // The type of the key (e.g., "GoldenKey", "SilverKey")
        bool isCollected = false; // Whether the key has been collected

        static std::string getID() { return "Key"; }

        // Method to deserialize key data from JSON
        void deserialize(const nlohmann::json& data) override;
    };

}
