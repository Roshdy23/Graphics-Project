#pragma once
#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "../components/mesh-renderer.hpp"
#include "../components/player-controller.hpp"
#include "../components/health-controller.hpp"
#include "../components/key-collected.hpp"
#include "../components/key.hpp"
#include "../application.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>
#include <string>

namespace our
{
    class CollisionSystem
    {
        Entity *player;

    public:
        KeyComponent *inventoryComponent;
        void setPlayer(Entity *playerEntity)
        {
            this->player = playerEntity;
        }

        bool checkAABBCollision(Entity *a, Entity *b)
        {
            glm::vec3 apos = a->localTransform.position;
            glm::vec3 asize = a->size;
            glm::vec3 bpos = b->localTransform.position;
            glm::vec3 bsize = b->size;
            std::string name = (a->name != "") ? a->name : "nullptr";
            if (name == "nullptr")
                return 0;
            // std::cout << a->name << " Pos: " << apos.x << ", " << apos.y << ", " << apos.z << " | Size: "
            //           << asize.x << ", " << asize.y << ", " << asize.z << "\n";
            // std::cout << "player Pos: " << bpos.x << ", " << bpos.y << ", " << bpos.z << " | Size: "
            //           << bsize.x << ", " << bsize.y << ", " << bsize.z << "\n";
            float temp1 = std::abs(apos.x - bpos.x) * 2;
            float temp2 = std::abs(apos.y - bpos.y) * 2;
            float temp3 = std::abs(apos.z - bpos.z) * 2;
            // std::cout << "temp1: " << temp1 << ", temp2: " << temp2 << ", temp3: " << temp3 << "\n";
            // std::cout << "asize: " << asize.x + bsize.x << ", " << asize.y + bsize.y << ", " << asize.z + bsize.z << "\n";

            bool ok = (std::abs(apos.x - bpos.x) * 2 < (asize.x + bsize.x)) &&
                      (std::abs(apos.y - bpos.y) * 2 < (asize.y + bsize.y)) &&
                      (std::abs(apos.z - bpos.z) * 2 < (asize.z + bsize.z));
            // std::cout << "Collision check: " << ok << "\n";
            return ok;
        }

        int update(World *world)
        {
            for (auto &entity : world->getEntities())
            {
                if (entity->hidden || entity == player)
                    continue;

                if (checkAABBCollision(entity, player))
                {
                    std::string name = entity->name;
                    if (name == "heart" || name == "key" || name == "enemy"||name == "door")
                    {
                        std::cout << "Collision detected with entity: " << name << "\n";
                        if (name == "heart")
                        {
                            std::cout << "Health pack collision detected. Hiding entity...\n";
                            HealthComponent *healthComponent = player->getComponent<HealthComponent>();
                            if (healthComponent == nullptr)
                            {
                                std::cerr << "Health component not found on player entity.\n";
                                return 0; // No health component, no action taken
                            }
                            healthComponent->health += 10;
                            if (healthComponent->health > 100)
                            {
                                healthComponent->health = 100; // Cap the health
                            }
                            std::cout << "Player health increased to: " << healthComponent->health << "\n";
                            entity->hidden = true; // Or remove from the world
                        }
                        else if (name == "key")
                        {
                            inventoryComponent = player->getComponent<KeyComponent>();
                            if (inventoryComponent == nullptr)
                            {
                                std::cerr << "Inventory component not found on player entity.\n";
                                return 0; // No inventory component, no action taken
                            }
                            inventoryComponent->keysCollected += 1;
                            std::cerr << "playerer has keys now  ." << inventoryComponent->keysCollected << "\n";
                            entity->hidden = true; // Or remove from the world
                        }
                        else if (name == "door" )
                        {
                            if(player->getComponent<KeyComponent>()->keysCollected == 3){
                            std::cout << "Door unlocked! You can exit the game.\n";
                            return 1; // Game won: All keys collected and door is unlocked
                            }
                            else
                            return -1; // Player died

                        }
                        else if (name == "enemy")
                        {
                            HealthComponent *healthComponent = player->getComponent<HealthComponent>();
                            healthComponent->health -= 50;
                            if (healthComponent->health <= 0)
                            {
                                healthComponent->isAlive = false;
                                return -1; // Player died
                            }
                            std::cout << "Player health decreased to: " << healthComponent->health << "\n";
                            entity->hidden = true; // Or remove from the world
                        }
                    }
                }
            }
            return 0;
        }
    };
}