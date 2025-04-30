#pragma once
#include "../ecs/component.hpp"
#include <glm/glm.hpp>

namespace our {
    class LightComponent : public Component {
    public:
        // Light types as enum (better than #define)
        enum Type { DIRECTIONAL = 0, POINT = 1, SPOT = 2 };

        // Members with default values
        int lightType = DIRECTIONAL;
        glm::vec3 direction{0.0f, 0.0f, -1.0f};
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 color{1.0f, 1.0f, 1.0f};
        glm::vec3 attenuation{1.0f, 0.0f, 0.0f};
        glm::vec2 cone_angles{glm::radians(15.0f), glm::radians(30.0f)};
        float displacement = 0.0f;

        // Required Component implementations
        static std::string getID() { return "Lighting"; }
        void deserialize(const nlohmann::json& data) override;
        
        // Virtual destructor
        ~LightComponent() override = default;
    };
}