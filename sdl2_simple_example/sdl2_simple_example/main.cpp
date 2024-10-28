#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "imgui_impl_sdl2.h"
#include <GL/glu.h> // para la perspectiva de glu
#include <glm/gtc/matrix_transform.hpp> // Añadir esta línea para incluir glm::lookAt

#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1300, 800);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

float camera_angle = 0.0f; // ángulo de la cámara
const float camera_radius = 10.0f; // distancia de la cámara al origen

float camX = sin(camera_angle) * camera_radius;
float camZ = cos(camera_angle) * camera_radius;

glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 8.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = cameraPos - cameraTarget;

glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

float yaw = -90.0f; //apunta a -z
float pitch = 0.0f;
bool isMiddleButtonPressed = false;
int lastMouseX, lastMouseY;

// Parámetros de la cuadrícula
int grid_size = 20;  // Número de líneas en cada dirección (XZ)
float grid_spacing = 1.0f;  // Espacio entre las líneas de la cuadrícula

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);

    // Configurar la matriz de proyección
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1, 100.0);

    // Volver al modo de la matriz del modelo/vista
    glMatrixMode(GL_MODELVIEW);
}

static std::vector<vec3> vertices;
static std::vector<unsigned int> indices;

static void loadModel(const char* file) {
    const struct aiScene* scene = aiImportFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene) {
        fprintf(stderr, "Error en cargar el archivo: %s\n", aiGetErrorString());
        return;
    }
    printf("Numero de mallas: %u\n", scene->mNumMeshes);
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        printf("\nMalla %u:\n", i);

        // Almacenar vértices
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            vertices.emplace_back(vertex.x, vertex.y, vertex.z);
        }

        // Almacenar índices
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }
    aiReleaseImport(scene);
}

// Función para dibujar la cuadrícula en el plano XZ
static void draw_grid() {
    glLineWidth(1.2f); // Ancho de línea más fino para la cuadrícula
    glColor3f(0.7f, 0.7f, 0.7f); // Color gris claro para la cuadrícula

    glBegin(GL_LINES);
    for (int i = -grid_size; i <= grid_size; ++i) {
        // Líneas paralelas al eje X (a lo largo del Z)
        glVertex3f(i * grid_spacing, 0.0f, -grid_size * grid_spacing);
        glVertex3f(i * grid_spacing, 0.0f, grid_size * grid_spacing);

        // Líneas paralelas al eje Z (a lo largo del X)
        glVertex3f(-grid_size * grid_spacing, 0.0f, i * grid_spacing);
        glVertex3f(grid_size * grid_spacing, 0.0f, i * grid_spacing);
    }
    glEnd();
}

static void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    // Actualizamos la posición de la cámara usando coordenadas polares para rotar sobre el eje XZ con una altura fija
    float camX = sin(camera_angle) * camera_radius;
    float camZ = cos(camera_angle) * camera_radius;

    // Posicionar la cámara en altura (aérea) y rotando alrededor del origen
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
            cameraPos.x + cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
            cameraUp.x, cameraUp.y, cameraUp.z);

    // Dibujar la cuadrícula del suelo
    draw_grid();

    // Dibujar el modelo
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < indices.size(); i++) {
        const vec3& vertex = vertices[indices[i]];
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();
}

void processInput() {
    const float cameraSpeed = 0.05f;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    // Movimiento hacia adelante y hacia atrás
    if (state[SDL_SCANCODE_W]) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (state[SDL_SCANCODE_S]) {
        cameraPos -= cameraSpeed * cameraFront;
    }

    // Movimiento lateral (izquierda y derecha)
    if (state[SDL_SCANCODE_A]) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (state[SDL_SCANCODE_D]) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
}

void updateCameraDirection() {
    const float sensitivity = 0.1f;

    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
        if (!isMiddleButtonPressed) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            isMiddleButtonPressed = true;
        }

        //Calculamos el desplazamiento
        int xOffset = mouseX - lastMouseX;
        int yOffset = lastMouseY - mouseY;//Invertimos la y

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        xOffset *= sensitivity;
        yOffset *= sensitivity;

        //Ajustamos los ángulos
        yaw += xOffset;
        pitch += yOffset;

        //Actualizamos la dirección de la cámara
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
    else {
        isMiddleButtonPressed = false;
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
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    init_openGL();

    // Cargar el modelo
    loadModel("cube.fbx");

    while (window.processEvents() && window.isOpen()) {
        const auto t0 = hrclock::now();

        processInput();
        updateCameraDirection();
        display_func();
        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }

    return 0;
}
