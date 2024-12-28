#include "Importer.h"
#include <GL/glew.h>
#include <IL/il.h>
#include <filesystem>
#include <iostream>
#include <fstream>

Importer::Importer() {
    ilInit();
    std::cout << "DevIL Initialized" << std::endl;
}

Importer::~Importer() {
    // Cleanup resources
    for (auto mesh : meshes) {
        delete mesh;
    }
    glDeleteTextures(textureIDs.size(), textureIDs.data());
}

bool Importer::ImportFBX(const std::string& path) {
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | 
        aiProcess_GenNormals | 
        aiProcess_CalcTangentSpace);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Error loading FBX: " << importer.GetErrorString() << std::endl;
        return false;
    }

    currentPath = std::filesystem::path(path).parent_path().string();
    return ProcessNode(scene->mRootNode, scene);
}

bool Importer::ImportTexture(const std::string& path, GLuint& textureID, int& width, int& height) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);
    
    std::wstring wpath(path.begin(), path.end());
    if (!ilLoadImage(wpath.c_str())) {
        std::cout << "Failed to load texture: " << path << std::endl;
        ilDeleteImages(1, &imageID);
        return false;
    }

    width = ilGetInteger(IL_IMAGE_WIDTH);
    height = ilGetInteger(IL_IMAGE_HEIGHT);
    
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, 
                 IL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ilDeleteImages(1, &imageID);
    textureIDs.push_back(textureID);
    return true;
}

bool Importer::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ComponentMesh* newMesh = ProcessMesh(mesh, scene);
        if (newMesh) {
            meshes.push_back(newMesh);
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
    return true;
}

ComponentMesh* Importer::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    if (!mesh) return nullptr;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    vertices.reserve(mesh->mNumVertices);
    texCoords.reserve(mesh->mNumVertices);

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices.push_back(glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        ));

        if (mesh->mTextureCoords[0]) {
            texCoords.push_back(glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            ));
        }
        else {
            texCoords.push_back(glm::vec2(0.0f, 0.0f));
        }
    }

    // Process indices
    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Validate mesh data
    if (vertices.empty() || indices.empty()) {
        std::cout << "Invalid mesh data" << std::endl;
        return nullptr;
    }

    return new ComponentMesh(nullptr, vertices, texCoords, indices, 0);
}

std::vector<GLuint> Importer::LoadMaterialTextures(aiMaterial* mat, aiTextureType type) {
    std::vector<GLuint> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        std::string fullPath = currentPath + "/" + str.C_Str();
        GLuint textureID;
        int width, height;
        
        if (ImportTexture(fullPath, textureID, width, height)) {
            textures.push_back(textureID);
        }
    }
    return textures;
}

bool Importer::SaveMesh(const std::string& path, const ComponentMesh* mesh) {
    std::ofstream file(path + ".mesh", std::ios::binary);
    if (!file) return false;

    // Header
    const char* header = "MESH";
    uint32_t version = 1;
    file.write(header, 4);
    file.write(reinterpret_cast<const char*>(&version), sizeof(uint32_t));

    // Bounding Box
    glm::vec3 bbMin = mesh->GetBoundingBoxMin();
    glm::vec3 bbMax = mesh->GetBoundingBoxMax();
    file.write(reinterpret_cast<const char*>(&bbMin), sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(&bbMax), sizeof(glm::vec3));

    // Data counts
    uint32_t vertexCount = mesh->vertices.size();
    uint32_t indexCount = mesh->indices.size();
    file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&indexCount), sizeof(uint32_t));

    // Mesh data
    file.write(reinterpret_cast<const char*>(mesh->vertices.data()), vertexCount * sizeof(glm::vec3));
    file.write(reinterpret_cast<const char*>(mesh->texCoords.data()), vertexCount * sizeof(glm::vec2));
    file.write(reinterpret_cast<const char*>(mesh->indices.data()), indexCount * sizeof(uint32_t));

    return true;
}

bool Importer::LoadMesh(const std::string& path, ComponentMesh** mesh) {
    std::ifstream file(path + ".mesh", std::ios::binary);
    if (!file) return false;

    // Verify header
    char header[5] = {0};
    uint32_t version;
    file.read(header, 4);
    file.read(reinterpret_cast<char*>(&version), sizeof(uint32_t));
    if (strcmp(header, "MESH") != 0 || version != 1) return false;

    // Read bounding box
    glm::vec3 bbMin, bbMax;
    file.read(reinterpret_cast<char*>(&bbMin), sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(&bbMax), sizeof(glm::vec3));

    // Read counts
    uint32_t vertexCount, indexCount;
    file.read(reinterpret_cast<char*>(&vertexCount), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&indexCount), sizeof(uint32_t));

    // Read mesh data
    std::vector<glm::vec3> vertices(vertexCount);
    std::vector<glm::vec2> texCoords(vertexCount);
    std::vector<unsigned int> indices(indexCount);

    file.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(glm::vec3));
    file.read(reinterpret_cast<char*>(texCoords.data()), vertexCount * sizeof(glm::vec2));
    file.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(uint32_t));

    *mesh = new ComponentMesh(nullptr, vertices, texCoords, indices, 0);
    return true;
}

bool Importer::SaveTexture(const std::string& path, const ComponentTexture* texture) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    // Get texture data from OpenGL
    int width = texture->GetWidth();
    int height = texture->GetHeight();
    std::vector<unsigned char> pixels(width * height * 4);
    
    glBindTexture(GL_TEXTURE_2D, texture->textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Save using DevIL
    ilTexImage(width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, pixels.data());
    std::wstring wpath(path.begin(), path.end());
    ilSaveImage(wpath.c_str());
    
    ilDeleteImages(1, &imageID);
    return true;
}

bool Importer::LoadTexture(const std::string& path, ComponentTexture** texture) {
    GLuint textureID;
    int width, height;
    
    if (!ImportTexture(path + ".tex", textureID, width, height)) {
        return false;
    }

    *texture = new ComponentTexture(nullptr, textureID, width, height, path);
    return true;
}