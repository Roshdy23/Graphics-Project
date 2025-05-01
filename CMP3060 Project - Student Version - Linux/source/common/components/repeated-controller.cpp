#include "repeated-controller.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void RepeatControllerComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object()) return;
        speed = data.value("speed", speed);
        // floatAmplitude = data.value("floatAmplitude", floatAmplitude);
        initialOffsetZ = data.value("initialOffsetZ", initialOffsetZ);
        entityType = data.value("entityType", entityType);
    }
}