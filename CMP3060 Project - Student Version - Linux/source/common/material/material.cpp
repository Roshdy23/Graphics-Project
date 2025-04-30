#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 7) Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        if (texture && sampler)
        {
            glActiveTexture(GL_TEXTURE0);
            texture->bind();
            sampler->bind(0);
            shader->set("tex", 0);
        }
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void LightingMaterial::setup() const {
        TintedMaterial::setup();
    
        struct TextureBinding {
            const Texture2D* texture;
            const char* uniformName;
            GLenum textureUnit;
        };
    
        const TextureBinding bindings[] = {
            {albedo, "material.albedo", GL_TEXTURE0},
            {specular, "material.specular", GL_TEXTURE1},
            {emissive, "material.emissive", GL_TEXTURE2},
            {roughness, "material.roughness", GL_TEXTURE3},
            {ambient_occlusion, "material.ambient_occlusion", GL_TEXTURE4}
        };
    
        for (int i = 0; i < 5; ++i) {
            const auto& binding = bindings[i];
            if (binding.texture) {
                glActiveTexture(binding.textureUnit);
                binding.texture->bind();
                sampler->bind(i);
                shader->set(binding.uniformName, i);
            }
        }
    }
    
    void LightingMaterial::deserialize(const nlohmann::json& data) {
        TintedMaterial::deserialize(data);
        if (!data.is_object()) return;
    
        albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));
        specular = AssetLoader<Texture2D>::get(data.value("specular", ""));
        emissive = AssetLoader<Texture2D>::get(data.value("emissive", ""));
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }    

}