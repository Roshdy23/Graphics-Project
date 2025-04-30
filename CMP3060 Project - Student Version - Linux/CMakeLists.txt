cmake_minimum_required(VERSION 3.10)  # Updated to support C++17 properly
project(GFX-LAB VERSION 0.1.0)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# GLFW build options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

# Add GLFW
add_subdirectory(vendor/glfw)

# Select GLAD source depending on OS
set(GLAD_SOURCE vendor/glad/src/gl.c)
if(WIN32)
    set(GLAD_SOURCE vendor/glad-windows/src/gl.c)
    include_directories(vendor/glad-windows/include)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Detected Linux")
    set(GLAD_SOURCE vendor/glad/src/gl.c)
    include_directories(vendor/glad/include)
else()
    message(WARNING "Unsupported OS")
endif()

# ImGui sources
set(IMGUI_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_impl/imgui_impl_glfw.cpp
    vendor/imgui/imgui_impl/imgui_impl_opengl3.cpp
)

# Vendor sources
set(VENDOR_SOURCES ${GLAD_SOURCE} ${IMGUI_SOURCES})

# Common sources
set(COMMON_SOURCES
    source/common/application.hpp
    source/common/application.cpp
    source/common/input/keyboard.hpp
    source/common/input/mouse.hpp

    source/common/asset-loader.cpp
    source/common/asset-loader.hpp
    source/common/deserialize-utils.hpp

    source/common/shader/shader.hpp
    source/common/shader/shader.cpp

    source/common/mesh/vertex.hpp
    source/common/mesh/mesh.hpp
    source/common/mesh/mesh-utils.hpp
    source/common/mesh/mesh-utils.cpp

    source/common/texture/sampler.hpp
    source/common/texture/sampler.cpp
    source/common/texture/texture2d.hpp
    source/common/texture/texture-utils.hpp
    source/common/texture/texture-utils.cpp
    source/common/texture/screenshot.hpp
    source/common/texture/screenshot.cpp

    source/common/material/pipeline-state.hpp
    source/common/material/pipeline-state.cpp
    source/common/material/material.hpp
    source/common/material/material.cpp
    source/common/components/light.hpp
    source/common/components/light.cpp
    source/common/ecs/component.hpp
    source/common/ecs/transform.hpp
    source/common/ecs/transform.cpp
    source/common/ecs/entity.hpp
    source/common/ecs/entity.cpp
    source/common/ecs/world.hpp
    source/common/ecs/world.cpp

    source/common/components/camera.hpp
    source/common/components/camera.cpp
    source/common/components/mesh-renderer.hpp
    source/common/components/mesh-renderer.cpp
    source/common/components/free-camera-controller.hpp
    source/common/components/free-camera-controller.cpp
    source/common/components/movement.hpp
    source/common/components/movement.cpp
    source/common/components/component-deserializer.hpp

    source/common/components/health-controller.hpp
    source/common/components/health-controller.cpp
    source/common/components/collision.hpp
    source/common/components/collision.cpp
    source/common/components/key.hpp
    source/common/components/key.cpp
    source/common/components/player-controller.hpp
    source/common/components/player-controller.cpp
    source/common/components/key-collected.hpp
    source/common/components/key-collected.cpp
    source/common/components/repeated-controller.cpp
    source/common/components/repeated-controller.hpp

    source/common/systems/forward-renderer.hpp
    source/common/systems/forward-renderer.cpp
    source/common/systems/free-camera-controller.hpp
    source/common/systems/movement.hpp
    source/common/systems/collision.hpp
    source/common/systems/player-controller.hpp
    source/common/systems/repeated-controller.hpp
)

# Include directories
include_directories(
    source/common
    vendor/glfw/include
    vendor/glm
    vendor/imgui
    vendor/utils
)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

# State sources
set(STATES_SOURCES
    source/states/play-state.hpp
    source/states/menu-state.hpp
    source/states/shader-test-state.hpp
    source/states/mesh-test-state.hpp
    source/states/transform-test-state.hpp
    source/states/pipeline-test-state.hpp
    source/states/texture-test-state.hpp
    source/states/sampler-test-state.hpp
    source/states/material-test-state.hpp
    source/states/entity-test-state.hpp
    source/states/renderer-test-state.hpp
)

# Define executable
add_executable(GAME_APPLICATION source/main.cpp ${STATES_SOURCES} ${COMMON_SOURCES} ${VENDOR_SOURCES})

# Link libraries
target_link_libraries(GAME_APPLICATION glfw)
target_link_libraries(GAME_APPLICATION OpenGL::GL)
