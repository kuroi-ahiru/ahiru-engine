#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

class Importer
{
    public:
        Importer();
        ~Importer();

        // Model importing
        bool ImportFBX(const std::string& path);
        
        // Texture importing
        bool ImportTexture(const std::string& path, GLuint& textureID, int& width, int& height);

        // Save/Load functionality
        bool SaveMesh(const std::string& path, const ComponentMesh* mesh);
        bool LoadMesh(const std::string& path, ComponentMesh** mesh);
        bool SaveTexture(const std::string& path, const ComponentTexture* texture);
        bool LoadTexture(const std::string& path, ComponentTexture** texture);

    private:
    // Assimp importer instance
    Assimp::Importer importer;

    // Helper methods for processing imported data
    bool ProcessNode(aiNode* node, const aiScene* scene);
    ComponentMesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<GLuint> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
    
    // Current import path for relative texture loading
    std::string currentPath;
    
    // Temporary storage for processed data
    std::vector<ComponentMesh*> meshes;
    std::vector<GLuint> textureIDs;
};