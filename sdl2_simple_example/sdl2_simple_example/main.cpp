#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "imgui_impl_sdl2.h"
#include <GL/glu.h> // para la perspectiva de glu

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

float camera_angle = 0.0f; // �ngulo de la c�mara
const float camera_radius = 10.0f; // distancia de la c�mara al origen

float camX = sin(camera_angle) * camera_radius;
float camZ = cos(camera_angle) * camera_radius;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = cameraPos - cameraTarget;

glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

glm::mat4 view;

// Par�metros de la cuadr�cula
int grid_size = 20;  // N�mero de l�neas en cada direcci�n (XZ)
float grid_spacing = 1.0f;  // Espacio entre las l�neas de la cuadr�cula

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);

    // Configurar la matriz de proyecci�n
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

        // Almacenar v�rtices
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            vertices.emplace_back(vertex.x, vertex.y, vertex.z);
        }

        // Almacenar �ndices
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }
    aiReleaseImport(scene);
}

// Funci�n para dibujar la cuadr�cula en el plano XZ
static void draw_grid() {
    glLineWidth(1.2f); // Ancho de l�nea m�s fino para la cuadr�cula
    glColor3f(0.7f, 0.7f, 0.7f); // Color gris claro para la cuadr�cula

    glBegin(GL_LINES);
    for (int i = -grid_size; i <= grid_size; ++i) {
        // L�neas paralelas al eje X (a lo largo del Z)
        glVertex3f(i * grid_spacing, 0.0f, -grid_size * grid_spacing);
        glVertex3f(i * grid_spacing, 0.0f, grid_size * grid_spacing);

        // L�neas paralelas al eje Z (a lo largo del X)
        glVertex3f(-grid_size * grid_spacing, 0.0f, i * grid_spacing);
        glVertex3f(grid_size * grid_spacing, 0.0f, i * grid_spacing);
    }
    glEnd();
}

static void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    // Actualizamos la posici�n de la c�mara usando coordenadas polares para rotar sobre el eje XZ con una altura fija
    float camX = sin(camera_angle) * camera_radius;
    float camZ = cos(camera_angle) * camera_radius;

    // Posicionar la c�mara en altura (a�rea) y rotando alrededor del origen
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Dibujar la cuadr�cula del suelo
    draw_grid();

    // Dibujar el modelo
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < indices.size(); i++) {
        const vec3& vertex = vertices[indices[i]];
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();

    // Incrementar el �ngulo de la c�mara para rotar
    camera_angle += 0.005f;  // Ajusta la velocidad de rotaci�n si es necesario
    if (camera_angle > 2.0f * 3.14159265f) camera_angle = 0.0f;  // Resetea el �ngulo despu�s de 360 grados
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
        display_func();
        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }

    return 0;
}
