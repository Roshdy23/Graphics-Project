#pragma once
#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "../components/mesh-renderer.hpp"
#include "../components/player-controller.hpp"
#include "../components/health-controller.hpp"
#include "../components/key-collected.hpp"
#include "../application.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>
#include <string>

namespace our {
    class CollisionSystem {
        Entity *player;
    
    public:
        void setPlayer(Entity* playerEntity) {
            this->player = playerEntity;
        }
    
        bool checkAABBCollision(Entity* a, Entity* b) {
            glm::vec3 apos = a->localTransform.position;
            glm::vec3 asize = a->size;
            glm::vec3 bpos = b->localTransform.position;
            glm::vec3 bsize = b->size;
    
            return (std::abs(apos.x - bpos.x) * 2 < (asize.x + bsize.x)) &&
                   (std::abs(apos.y - bpos.y) * 2 < (asize.y + bsize.y)) &&
                   (std::abs(apos.z - bpos.z) * 2 < (asize.z + bsize.z));
        }
    
        int update(World* world) {
            for (auto& entity : world->getEntities()) {
                if (entity->hidden || entity == player) continue;
    
                if (checkAABBCollision(entity, player)) {
                    std::string name = entity->name;
    
                    if (name == "health_pack") {
                        HealthComponent* healthComponent = player->getComponent<HealthComponent>();
                        healthComponent->health += 10;
                        if (healthComponent->health > 100) {
                            healthComponent->health = 100; // Cap the health
                        }
                        entity->hidden = true; // Or remove from the world
                    } 
                    else if (name == "key") {
                        PlayerInventoryComponent* inventoryComponent = player->getComponent<PlayerInventoryComponent>();
                        inventoryComponent->keysCollected += 1;
                        entity->hidden = true; // Or remove from the world
                    } 
                    else if (name == "door" && player->getComponent<PlayerInventoryComponent>()->keysCollected == 3) {
                        return 1; // Game won: All keys collected and door is unlocked
                    }
                    else if (name == "enemy") {
                        HealthComponent* healthComponent = player->getComponent<HealthComponent>();
                        healthComponent->health -= 25;
                        if (healthComponent->health <= 0) {
                            healthComponent->isAlive = false;
                            return -1; // Player died
                        }
                    }
                }
            }
            return 0;
        }
    };
    }