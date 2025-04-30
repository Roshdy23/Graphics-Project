#include "light.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <asset-loader.hpp>
#include "../deserialize-utils.hpp"
#include <iostream>
namespace our
{
    void LightComponent::deserialize(const nlohmann::json &data)
    {
        // Check if the JSON data is an object
        if (!data.is_object())
            return;
         // Read the "lightType" value from the JSON object or use the default value from the member variable
        lightType = data.value("lightType", lightType);

        // Read the "position" value from the JSON object or use the default value from the member variable
        position = data.value("position", glm::vec3(0.0f, 0.0f, 0.0f));

        // Read the "direction" value from the JSON object or use the default value from the member variable
        direction = data.value("direction", glm::vec3(0.0f, 0.0f, 0.0f));

        // Read the "color" value from the JSON object or use the default value from the member variable
        color = data.value("color",  glm::vec3(0.0f, 0.0f, 0.0f));

        // Read the "attenuation" value from the JSON object or use the default value from the member variable
        attenuation = data.value("attenuation", glm::vec3(0.0f, 0.0f, 0.0f));

        // Read the "cone_angles" value from the JSON object or use the default value from the member variable
        cone_angles = data.value("cone_angles", glm::vec2(0.0f, 0.0f));

        // Read the "displacement" value from the JSON object or use the default value from the member variable
        displacement = data.value("displacement", 0.0f);
        
    
    }
    LightComponent::~LightComponent() {
        // Can be empty, but must exist
    }
}