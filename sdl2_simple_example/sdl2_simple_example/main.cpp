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

static const ivec2 WINDOW_SIZE(1300, 800);
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
float pitch = glm::degrees(asin(cameraFront.y));
bool isRightButtonPressed = false;
int lastMouseX, lastMouseY;

float fov = 45.0f;

// Par�metros de la cuadr�cula
int grid_size = 20;
float grid_spacing = 1.5f;

static std::vector<vec3> vertices;
static std::vector<vec2> texCoords;
static std::vector<unsigned int> indices;

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

static void loadModel(const char* file) {

    const struct aiScene* scene = aiImportFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        fprintf(stderr, "Error al cargar el archivo: %s\n", aiGetErrorString());
        return;
    }
    printf("N�mero de mallas: %u\n", scene->mNumMeshes);

    vertices.clear();
    texCoords.clear();
    indices.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        printf("\nMalla %u:\n", i);

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
}   

GLuint LoadTexture(const char* file) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (ilLoad(IL_TYPE_UNKNOWN, (wchar_t*)file)) {
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
        printf("Cargada textura correctamente \n");
    }
    else {
        fprintf(stderr, "Error al cargar la textura.\n");
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

static void draw_grid() {
    glLineWidth(1.2f);
    glColor3f(0.7f, 0.7f, 0.7f);

    glBegin(GL_LINES);
    for (int i = -grid_size; i <= grid_size; ++i) {
        glVertex3f(i * grid_spacing, 0.0f, -grid_size * grid_spacing);
        glVertex3f(i * grid_spacing, 0.0f, grid_size * grid_spacing);

        glVertex3f(-grid_size * grid_spacing, 0.0f, i * grid_spacing);
        glVertex3f(grid_size * grid_spacing, 0.0f, i * grid_spacing);
    }
    glEnd();
}

static void display_func(GLuint textureID) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    // Posicionar la cámara en altura (aérea) y rotando alrededor del origen
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
        cameraPos.x + cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
        cameraUp.x, cameraUp.y, cameraUp.z);
    draw_grid();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < indices.size(); i++) {
        const vec3& vertex = vertices[indices[i]];
        const vec2& uv = texCoords[indices[i]];
        glTexCoord2f(uv.x, uv.y);
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void updateCamera() {
    const float sensitivity = 0.1f;

    const float cameraSpeed = 0.05f;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        if (!isRightButtonPressed) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            isRightButtonPressed = true;
        }

        //Calculamos el desplazamiento
        float xOffset = mouseX - lastMouseX;
        float yOffset = lastMouseY - mouseY;//Invertimos la y

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
    }
    else {
        isRightButtonPressed = false;
    }
}

void updateZoom(SDL_Event& event) {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    
    if (event.type == SDL_MOUSEWHEEL) {
        printf("rueda detectada ");
        if (event.wheel.y > 0) { // Scroll hacia arriba
            //Imprime el valor de fov para depuración
            printf("rueda arriba ");
            fov -= 1.0f;
        }
        else if (event.wheel.y < 0) { // Scroll hacia abajo
            //Imprime el valor de fov para depuración
            printf("rueda abajo ");
            fov += 1.0f;
        }
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

static bool processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            return false;
            break;
        default:
            ImGui_ImplSDL2_ProcessEvent(&event);
            updateZoom(event);
            break;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    MyWindow window("Ahiru Engine", WINDOW_SIZE.x, WINDOW_SIZE.y);
    ilInit();
    init_openGL();

    GLuint textureID = LoadTexture("Baker_house.png");
    loadModel("BakerHouse.fbx");
    
    SDL_Event event;
    while (window.processEvents() && window.isOpen()) {
        const auto t0 = hrclock::now();

        updateCamera();
        updateZoom(event);
        display_func(textureID);
        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }

    return 0;
}
