#include "Scene.h"
#include <IL/il.h> // Biblioteca DevIL para carga de texturas
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "ComponentMesh.h"

Scene::Scene() {
    ilInit(); // Inicializa DevIL para manejo de texturas
}

Scene::~Scene() {
    // No necesitas limpiar `gameObjects`, std::shared_ptr se encarga de esto automáticamente
}

void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(std::shared_ptr<GameObject> gameObject) {
    gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
}

void Scene::Render() {
    for (auto& gameObject : gameObjects) {
        if (gameObject) {
            gameObject->Render();
        }
    }
}

void Scene::Update() {
    for (auto& gameObject : gameObjects) {
        if (gameObject) {
            gameObject->Update();
        }
    }
}

bool Scene::LoadModel(const char* modelFile, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& texCoords, std::vector<unsigned int>& indices) {
    const aiScene* scene = aiImportFile(modelFile, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        fprintf(stderr, "Error al cargar el archivo: %s\n", aiGetErrorString());
        return false;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            vertices.emplace_back(vertex.x, vertex.y, vertex.z);
        }

        if (mesh->HasTextureCoords(0)) {
            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
                aiVector3D uv = mesh->mTextureCoords[0][v];
                texCoords.emplace_back(fmodf(uv.x, 1.0f), fmodf(uv.y, 1.0f));
            }
        }
        else {
            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
                texCoords.emplace_back(0.0f, 0.0f);
            }
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    aiReleaseImport(scene);
    return true;
}

GLuint Scene::LoadTexture(const char* file) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (ilLoad(IL_TYPE_UNKNOWN, (wchar_t*)file)) {
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    }
    else {
        fprintf(stderr, "Error al cargar la textura.\n");
        ilDeleteImages(1, &imageID);
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

    ilDeleteImages(1, &imageID);
    return textureID;
}

std::shared_ptr<GameObject> Scene::CreateGameObject(const char* modelFile, const char* textureFile) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    if (!LoadModel(modelFile, vertices, texCoords, indices)) {
        fprintf(stderr, "No se pudo cargar el modelo: %s\n", modelFile);
        return nullptr;
    }

    GLuint textureID = LoadTexture(textureFile);
    if (textureID == 0) {
        fprintf(stderr, "No se pudo cargar la textura: %s\n", textureFile);
        return nullptr;
    }

    auto gameObject = std::make_shared<GameObject>(modelFile);
    auto meshComponent = std::make_shared<ComponentMesh>(gameObject.get(), vertices, texCoords, indices, textureID);
    gameObject->AddComponent(meshComponent);

    return gameObject;
}

void Scene::DrawGrid(int grid_size, float grid_spacing) {
    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    glLineWidth(1.2f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.1f);

    glBegin(GL_LINES);
    for (int i = -grid_size; i <= grid_size; ++i) {
        glVertex3f(i * grid_spacing, 0.0f, -grid_size * grid_spacing);
        glVertex3f(i * grid_spacing, 0.0f, grid_size * grid_spacing);

        glVertex3f(-grid_size * grid_spacing, 0.0f, i * grid_spacing);
        glVertex3f(grid_size * grid_spacing, 0.0f, i * grid_spacing);
    }
    glEnd();

    glDisable(GL_BLEND);
    glPopAttrib();
}
