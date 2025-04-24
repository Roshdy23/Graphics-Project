#pragma once

#include "../ecs/component.hpp"
#include <glm/glm.hpp>

namespace our {

    // The CollisionComponent stores data necessary for basic AABB collision detection
    class CollisionComponent : public Component {
    public:
        glm::vec3 size = {1.0f, 1.0f, 1.0f}; // Width, height, depth of bounding box
        bool isTrigger = false;              // True if collider triggers events without physical blocking

        static std::string getID() { return "Collision"; }

        void deserialize(const nlohmann::json& data) override;
    };

}
