#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <systems/player-controller.hpp>
#include <systems/repeated-controller.hpp>
#include <asset-loader.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State
{

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionSystem collisionSystem;
    our::PlayerControllerSystem playerControllerSystem;
    our::RepeatControllerSystem repeatControllerSystem;

    void onInitialize() override
    {
        // First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets"))
        {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world"))
        {
            world.deserialize(config["world"]);
        }
        /////////////////////////////////////
        for (auto entity : world.getEntities())
        {
            if (entity->name == "player")
            {
                playerControllerSystem.setPlayer(entity);
                collisionSystem.setPlayer(entity); // ✅ Optional

                break;
            }
        }
        for (auto entity : world.getEntities())
        {
            if (entity->name == "Camera")
            {
                playerControllerSystem.setCamera(entity);
                repeatControllerSystem.setCamera(entity); // ✅ Optional
                break;
            }
        }

        // Provide app pointer to systems that need it
        playerControllerSystem.enter(getApp());

        ///////////////////////////////////////////////////////////////////
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"], playerControllerSystem.getPlayer());
    }

    void onDraw(double deltaTime) override
    {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);
        playerControllerSystem.update(&world, (float)deltaTime); // ✅ NEW
        repeatControllerSystem.update(&world, (float)deltaTime); // ✅ NEW
        int collisionResult = collisionSystem.update(&world);    // ✅ Optional
        if (collisionResult == 1)
        {
            // Game won
            getApp()->changeState("win");
        }
        else if (collisionResult == -1)
        {
            // Game lost
            getApp()->changeState("lose");
        }

        renderer.render(&world);

        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();

        if (keyboard.justPressed(GLFW_KEY_ESCAPE))
        {
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
    }

    void onDestroy() override
    {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};