#include "ModelImporter.h"
#include <iostream>


bool ModelImporter::LoadModel(const char* file) {
    const aiScene* scene = aiImportFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        std::cerr << "Error al cargar el archivo: " << aiGetErrorString() << std::endl;
        return false;
    }

    vertices.clear();
    texCoords.clear();
    indices.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // Cargar vert
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            vertices.emplace_back(vertex.x, vertex.y, vertex.z);
        }

        // Cargar coordenadas UV
        if (mesh->HasTextureCoords(0)) {
            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
                aiVector3D uv = mesh->mTextureCoords[0][v];
                texCoords.emplace_back(fmodf(uv.x, 1.0f), fmodf(uv.y, 1.0f));
            }
        }
        else {
            texCoords.resize(vertices.size(), glm::vec2(0.0f, 0.0f));
        }

        // Cargar indices
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }
    std::cout << "Vertices cargados: " << vertices.size() << std::endl;
    std::cout << "Coordenadas UV cargadas: " << texCoords.size() << std::endl;
    std::cout << "Indices cargados: " << indices.size() << std::endl;

    aiReleaseImport(scene);
    return true;
}