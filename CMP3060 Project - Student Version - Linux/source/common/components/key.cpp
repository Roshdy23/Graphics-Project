#include "key.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {

    void KeyComponent::deserialize(const nlohmann::json& data) {
        if (!data.is_object()) return;

        // Deserialize the key type and collection status
        keyType = data.value("keyType", keyType);  // Default: empty string if not found
        isCollected = data.value("isCollected", isCollected); // Default: false if not found
    }

}
