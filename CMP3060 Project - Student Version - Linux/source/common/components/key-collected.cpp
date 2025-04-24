#include "key-collected.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void PlayerInventoryComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        keysCollected = data.value("keysCollected", keysCollected);
    }
}
