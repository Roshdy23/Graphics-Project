#include "repeated-controller.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void RepeatControllerComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object()) return;
        speed = data.value("speed", speed);
        floatAmplitude = data.value("floatAmplitude", floatAmplitude);
        floatSpeed = data.value("floatSpeed", floatSpeed);
        entityType = data.value("entityType", entityType);
    }
}