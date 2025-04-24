#pragma once

#include "../ecs/component.hpp"

namespace our {

    class PlayerInventoryComponent : public Component {
    public:
        int keysCollected = 0;

        static std::string getID() { return "PlayerInventory"; }

        void deserialize(const nlohmann::json& data) override;
    };

}
