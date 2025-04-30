#pragma once
 
 #include "../ecs/component.hpp"
 
 #include <glm/glm.hpp>
 
 namespace our
 {
 
     class LightComponent : public Component
     {
     public:
         // TODO: Add more members if needed
         int lightType;         // Type of the light: 0 for directional, 1 for point, 2 for spot
         glm::vec3 direction;   // Direction of the light (for directional light)
         glm::vec3 position;    // Position of the light (for point light)
         glm::vec3 color;       // Ambient and specular color of the light
         glm::vec3 attenuation; // Attenuation factors for the light (controls falloff)
         glm::vec2 cone_angles; // Cone angles for spot lighting (inner and outer angles)
         float displacement;    // Displacement of the light (for point light)
         
         #define DIRECTIONAL 0
         #define POINT       1
         #define SPOT        2
 
         // The ID of this component type is "Lighting"
         static std::string getID() { return "Lighting"; }
 
         // Reads light component data from the given JSON object
         void deserialize(const nlohmann::json &data) override;
         virtual ~LightComponent();
 
     };
 
 }