#include "Scene.h"
#include <IL/il.h> 
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentTexture.h"


//funcion aux para rayo picking mouse
static bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
    const glm::vec3& boxMin, const glm::vec3& boxMax) {
    float tmin = (boxMin.x - rayOrigin.x) / rayDir.x;
    float tmax = (boxMax.x - rayOrigin.x) / rayDir.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (boxMin.y - rayOrigin.y) / rayDir.y;
    float tymax = (boxMax.y - rayOrigin.y) / rayDir.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (boxMin.z - rayOrigin.z) / rayDir.z;
    float tzmax = (boxMax.z - rayOrigin.z) / rayDir.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    return true;
}


Scene::Scene() {
    ilInit();
}

Scene::~Scene() {
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

            // debug AABB
            if (debugMode) {
                gameObject->CalculateAABB();
                gameObject->DrawAABB();
            }
        }
    }

    //Debug visual del rayo, no me va pero lo dejo por si da tiempo a arreglarlo
    if (debugMode) {
        // depuracion purposes
        std::cout << "Drawing debug ray..." << std::endl;
        std::cout << "Ray Origin: (" << debugRayOrigin.x << ", " << debugRayOrigin.y << ", " << debugRayOrigin.z << ")\n";
        std::cout << "Ray Direction: (" << debugRayDir.x << ", " << debugRayDir.y << ", " << debugRayDir.z << ")\n";
               
        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT);
        glDisable(GL_LIGHTING);
        glLineWidth(2.0f);
        glColor3f(1.0f, 0.0f, 0.0f); //Rojo

        glm::vec3 rayEnd = debugRayOrigin + debugRayDir * 1000.0f; //Longitud del rayo (100 unidades)

        glBegin(GL_LINES);
        glVertex3f(debugRayOrigin.x, debugRayOrigin.y, debugRayOrigin.z);
        glVertex3f(rayEnd.x, rayEnd.y, rayEnd.z);
        glEnd();

        glPopAttrib();
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

    auto transformComponent = std::make_shared<ComponentTransform>(gameObject.get());
    gameObject->AddComponent(transformComponent);

    auto textureComponent = std::make_shared<ComponentTexture>(gameObject.get(), textureID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), textureFile);
    gameObject->AddComponent(textureComponent);

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

std::shared_ptr<GameObject> Scene::PickGameObject(const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
    debugRayOrigin = rayOrigin; //guardar datos del rayo para debug
    debugRayDir = rayDir;

    std::shared_ptr<GameObject> closestObject = nullptr;
    float closestDistance = std::numeric_limits<float>::max();

    for (const auto& gameObject : gameObjects) {
        // Suponemos que cada GameObject tiene GetBoundingBoxMin() y GetBoundingBoxMax()
        const glm::vec3 boxMin = gameObject->GetBoundingBoxMin();
        const glm::vec3 boxMax = gameObject->GetBoundingBoxMax();

        if (RayIntersectsAABB(rayOrigin, rayDir, boxMin, boxMax)) {
            float distance = glm::distance(rayOrigin, gameObject->GetPosition());
            if (distance < closestDistance) {
                closestDistance = distance;
                closestObject = gameObject;
            }
        }
    }

    selectedGameObject = closestObject; //actualizamos el seleccionado
    return closestObject;
}
