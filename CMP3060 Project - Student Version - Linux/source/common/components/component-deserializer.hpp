#pragma once

#include "../ecs/entity.hpp"
#include "camera.hpp"
#include "mesh-renderer.hpp"
#include "free-camera-controller.hpp"
#include "movement.hpp"
///
#include "health-controller.hpp"
#include "key-collected.hpp"
#include "key.hpp"
#include "player-controller.hpp"
#include "collision.hpp"
#include "repeated-controller.hpp"
#include "light.hpp"

namespace our
{

    // Given a json object, this function picks and creates a component in the given entity
    // based on the "type" specified in the json object which is later deserialized from the rest of the json object
    inline void deserializeComponent(const nlohmann::json &data, Entity *entity)
    {
        std::string type = data.value("type", "");
        Component *component = nullptr;
        // TODO: (Req 8) Add an option to deserialize a "MeshRendererComponent" to the following if-else statement
        if (type == CameraComponent::getID())
        {
            component = entity->addComponent<CameraComponent>();
        }
        else if (type == LightComponent::getID())
        {
            component = entity->addComponent<LightComponent>();
        }
        
        else if (type == FreeCameraControllerComponent::getID())
        {
            component = entity->addComponent<FreeCameraControllerComponent>();
        }
        else if (type == MovementComponent::getID())
        {
            component = entity->addComponent<MovementComponent>();
        }
        else if (type == MeshRendererComponent::getID())
        { // Add MeshRendererComponent here
            component = entity->addComponent<MeshRendererComponent>();
        }
        else if (type == HealthComponent::getID())  // For Health System
        {
            component = entity->addComponent<HealthComponent>();
        }
        else if (type == PlayerInventoryComponent::getID())  // For collecting items
        {
            component = entity->addComponent<PlayerInventoryComponent>();
        }
        else if (type == KeyComponent::getID())  // For Enemy AI
        {
            component = entity->addComponent<KeyComponent>();
        }
        else if (type == PlayerControllerComponent::getID())  // For puzzles and challenges
        {
            component = entity->addComponent<PlayerControllerComponent>();
        }
        else if (type == CollisionComponent::getID())  // For puzzles and challenges
        {
            component = entity->addComponent<CollisionComponent>();
        }
        else if (type == RepeatControllerComponent::getID())  // For puzzles and challenges
        {
            component = entity->addComponent<RepeatControllerComponent>();
        }

        if (component)
            component->deserialize(data);
    }

}