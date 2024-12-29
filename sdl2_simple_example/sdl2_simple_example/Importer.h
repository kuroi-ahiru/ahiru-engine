#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include "ComponentMesh.h"

class Importer {
public:
struct Model {
        std::string name;
        std::vector<ComponentMesh> meshes;
    };

    struct Texture {
        GLuint textureID = 0;
        int width = 0;
        int height = 0;
        int channels = 0;
        std::string name;
    };

    Importer();
    ~Importer();

    bool Init();

    bool ImportFBX(const std::string& file_path);
    bool ImportTexture(const std::string& filePath);

    bool SaveModelToCustomFormat(const std::string& modelName, const std::string& outputPath);
    bool LoadModelFromCustomFormat(const std::string& filePath);

    bool SaveTextureToCustomFormat(const std::string& textureName, unsigned char* data, const Texture& texture);
    bool LoadTextureFromCustomFormat(const std::string& filePath);

    const std::string GetTextureName(const std::string& filepath)const;
    const Texture* GetTexture(const std::string& textureName)const;

    const std::unordered_map<std::string, Model>& GetModels() const { return models; }
    const Model* GetModel(const std::string& modelName) const;
    const std::string GetModelName(const std::string& filePath) const;
    const Texture& GetTexture() const { return texture; }
    GLuint GetTextureID() const { return texture.textureID; }

private:
    bool CreateRequiredDirectories();

    bool ProcessNode(aiNode* node, const aiScene* scene, Model& model);
    bool ProcessMesh(aiMesh* mesh, const aiScene* scene, Model& model);

    std::unordered_map<std::string, Model> models;
    std::unordered_map<std::string, Texture> textures;
    Texture texture;
    std::string assetsPath;
    std::string libraryPath;
};