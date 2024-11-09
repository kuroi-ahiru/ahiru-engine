#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "imgui_impl_sdl2.h"
#include <IL/il.h>
#include <GL/glu.h> // para la perspectiva de glu
#include <glm/gtc/matrix_transform.hpp> // A�adir esta l�nea para incluir glm::lookAt

#include "GameObject.h"
#include "ComponentMesh.h"

#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec2 = glm::vec2;
using vec3 = glm::vec3;

static const ivec2 WINDOW_SIZE(1200, 600); //1300 800 para poder trabajr en el portatil luego se cambia
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

float camera_angle = 0.0f; // �ngulo de la c�mara
const float camera_radius = 10.0f; // distancia de la c�mara al origen

float camX = sin(camera_angle) * camera_radius;
float camZ = cos(camera_angle) * camera_radius;

glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 8.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = -cameraDirection;

float yaw = glm::degrees(atan2(cameraFront.z, cameraFront.x));
float initialyaw = glm::degrees(atan2(cameraFront.z, cameraFront.x));
float pitch = glm::degrees(asin(cameraFront.y));
float initialpitch = glm::degrees(asin(cameraFront.y));
bool isLeftButtonPressed = false;
bool isRightButtonPressed = false;
int lastMouseX, lastMouseY;

float fov = 45.0f;
bool rotation = false;

// Par�metros de la cuadr�cula
int grid_size = 30;
float grid_spacing = 1.5f;

//static std::vector<vec3> vertices; //Comento pq lo he puesto en funcion gameobject pero lo dejo por si al final lo cambio
//static std::vector<vec2> texCoords;
//static std::vector<unsigned int> indices;

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.15, 0.15, 0.15, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

//static void loadModel(const char* file) {
//
//    const struct aiScene* scene = aiImportFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);
//    if (!scene) {
//        fprintf(stderr, "Error al cargar el archivo: %s\n", aiGetErrorString());
//        return;
//    }
//    printf("N�mero de mallas: %u\n", scene->mNumMeshes);
//
//    vertices.clear();
//    texCoords.clear();
//    indices.clear();
//
//    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
//        aiMesh* mesh = scene->mMeshes[i];
//        printf("\nMalla %u:\n", i);
//
//        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
//            aiVector3D vertex = mesh->mVertices[v];
//            vertices.emplace_back(vertex.x, vertex.y, vertex.z);
//        }
//
//        if (mesh->HasTextureCoords(0)) {
//            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
//                aiVector3D uv = mesh->mTextureCoords[0][v];
//                texCoords.emplace_back(fmodf(uv.x, 1.0f), fmodf(uv.y, 1.0f));
//            }
//        }
//        else {
//            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
//                texCoords.emplace_back(0.0f, 0.0f);
//            }
//        }
//
//        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
//            aiFace face = mesh->mFaces[f];
//            for (unsigned int j = 0; j < face.mNumIndices; j++) {
//                indices.push_back(face.mIndices[j]);
//            }
//        }
//    }
//
//    aiReleaseImport(scene);
//}   

bool loadModel(const char* modelFile, std::vector<vec3>& vertices, std::vector<vec2>& texCoords, std::vector<unsigned int>& indices) {
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

//GLuint LoadTexture(const char* file) {
//    ILuint imageID;
//    ilGenImages(1, &imageID);
//    ilBindImage(imageID);
//
//    if (ilLoad(IL_TYPE_UNKNOWN, (wchar_t*)file)) {
//        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
//        printf("Cargada textura correctamente \n");
//    }
//    else {
//        fprintf(stderr, "Error al cargar la textura.\n");
//    }
//
//    GLuint textureID;
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
//        ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA,
//        GL_UNSIGNED_BYTE, ilGetData());
//
//    ilDeleteImages(1, &imageID);
//    return textureID;
//}

GLuint loadTexture(const char* file) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (ilLoad(IL_TYPE_UNKNOWN, (wchar_t*)file)) {
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        printf("Cargada textura correctamente \n");
    }
    else {
        fprintf(stderr, "Error al cargar la textura.\n");
        ilDeleteImages(1, &imageID);
        return 0; // Devuelve 0 si falla la carga
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
        ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA,
        GL_UNSIGNED_BYTE, ilGetData());

    ilDeleteImages(1, &imageID);
    return textureID;
}

// Create GameObject desde modelo y textura
std::shared_ptr<GameObject> createGameObject(const char* modelFile, const char* textureFile) {
    std::vector<vec3> vertices;
    std::vector<vec2> texCoords;
    std::vector<unsigned int> indices;

    if (!loadModel(modelFile, vertices, texCoords, indices)) {
        fprintf(stderr, "No se pudo cargar el modelo: %s\n", modelFile);
        return nullptr;
    }
        
    GLuint textureID = loadTexture(textureFile);
    if (textureID == 0) {
        fprintf(stderr, "No se pudo cargar la textura: %s\n", textureFile);
        return nullptr;
    }
        
    auto gameObject = std::make_shared<GameObject>(modelFile);
    auto meshComponent = std::make_shared<ComponentMesh>(gameObject.get(), vertices, texCoords, indices, textureID);
    gameObject->AddComponent(meshComponent);

    return gameObject;
}

static void draw_grid() {
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

//static void display_func(GLuint textureID) {
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glLoadIdentity();
//
//    // Posicionar la cámara en altura (aérea) y rotando alrededor del origen
//    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
//        cameraPos.x + cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
//        cameraUp.x, cameraUp.y, cameraUp.z);
//    draw_grid();
//
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glBegin(GL_TRIANGLES);
//    for (unsigned int i = 0; i < indices.size(); i++) {
//        const vec3& vertex = vertices[indices[i]];
//        const vec2& uv = texCoords[indices[i]];
//        glTexCoord2f(uv.x, uv.y);
//        glVertex3f(vertex.x, vertex.y, vertex.z);
//    }
//    glEnd();
//    glDisable(GL_TEXTURE_2D);
//}

void updateCamera() {
    const float sensitivity = 0.1f;

    const float cameraSpeed = 0.05f;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    int mouseX, mouseY;
	float xOffset, yOffset;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        if (!isRightButtonPressed) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            isRightButtonPressed = true;
            rotation = false;
        }

        //Calculamos el desplazamiento
        xOffset = mouseX - lastMouseX;
        yOffset = lastMouseY - mouseY;//Invertimos la y

        xOffset *= sensitivity;
        yOffset *= sensitivity;

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        //Ajustamos los �ngulos
        yaw += xOffset;
        pitch += yOffset;

		//Nos aseguramos de que el �ngulo de inclinaci�n no sea demasiado alto
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        //Actualizamos la direcci�n de la c�mara
        glm::vec3 newcameraDirection;
        newcameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newcameraDirection.y = sin(glm::radians(pitch));
        newcameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(newcameraDirection);

        // Movimiento hacia adelante y hacia atr�s
        if (state[SDL_SCANCODE_W]) {
            cameraPos += cameraSpeed * cameraFront;
            if (state[SDL_SCANCODE_LSHIFT])
            {
                cameraPos += 2 * cameraSpeed * cameraFront;
            }
        }
        if (state[SDL_SCANCODE_S]) {
            cameraPos -= cameraSpeed * cameraFront;
            if (state[SDL_SCANCODE_LSHIFT])
            {
                cameraPos -= 2 * cameraSpeed * cameraFront;
            }
        }

        // Movimiento lateral (izquierda y derecha)
        if (state[SDL_SCANCODE_A]) {
            cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
            if (state[SDL_SCANCODE_LSHIFT])
            {
                cameraPos -= 2 * cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
            }
        }
        if (state[SDL_SCANCODE_D]) {
            cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
            if (state[SDL_SCANCODE_LSHIFT])
            {
                cameraPos += 2 * cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
            }
        }

        //Zoom
        if (state[SDL_SCANCODE_Q]) { //Añadir zoom
                fov -= 1.0f;
        }
        else if (state[SDL_SCANCODE_E]) { //Quitar zoom
            fov += 1.0f;
        }
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 45.0f)
            fov = 45.0f;

        //Actualiza la perspectiva con el nuevo FOV
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fov, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
    }
    else if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        if (!isLeftButtonPressed) {
            isLeftButtonPressed = true;
        }
        if (state[SDL_SCANCODE_LALT]) { //Añadir zoom
            if (rotation == false)
            {
                rotation = true;
			}
            else
            {
                rotation = false;
            }
        }
    }
    else {
		isLeftButtonPressed = false;
        isRightButtonPressed = false;
    }

    //Centrar mirada al objeto
    if (state[SDL_SCANCODE_F]) {
        //Actualizamos la direcci�n de la c�mara
        cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - cameraPos);
		yaw = initialyaw;
		pitch = initialpitch;
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }

    if (rotation == true) {
        camera_angle += 0.005f;
        if (camera_angle > 2.0f * 3.14159265f) camera_angle = 0.0f;
        cameraPos.x = sin(camera_angle) * camera_radius;
        cameraPos.z = cos(camera_angle) * camera_radius;
        cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - cameraPos);
        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }

}
static bool processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            return false;
            break;
        default:
            ImGui_ImplSDL2_ProcessEvent(&event);
            break;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    MyWindow window("Ahiru Engine", WINDOW_SIZE.x, WINDOW_SIZE.y);
    ilInit();
    init_openGL();

    // Cargar modelo bakerhouse por defecto como GameObjects, pending to check
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    gameObjects.push_back(createGameObject("BakerHouse.fbx", "Baker_house.png"));

    while (window.processEvents() && window.isOpen()) {
        const auto t0 = hrclock::now();

        updateCamera();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
            cameraPos.x + cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
            cameraUp.x, cameraUp.y, cameraUp.z);

        draw_grid();

        // Renderizar todos los GameObjects
        for (auto& gameObject : gameObjects) {
            if (gameObject) {
                gameObject->Render();
            }
        }

        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) std::this_thread::sleep_for(FRAME_DT - dt);
    }

    return 0;
}