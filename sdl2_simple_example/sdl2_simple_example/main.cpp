#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "imgui_impl_sdl2.h"
#include <GL/glu.h>
#include <IL/il.h>
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

// Parámetros de la cámara
float camera_angle = 0.0f;
float camera_radius = 10.0f;
float camera_height = 8.0f;

// Parámetros de la cuadrícula
int grid_size = 20;
float grid_spacing = 1.5f;

static std::vector<vec3> vertices;
static std::vector<vec2> texCoords;
static std::vector<unsigned int> indices;

GLuint vao, vboVertices, vboTexCoords, ebo; // Buffers

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
        fprintf(stderr, "Error en cargar el archivo: %s\n", aiGetErrorString());
        return;
    }
    printf("Número de mallas: %u\n", scene->mNumMeshes);

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        printf("\nMalla %u:\n", i);

        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            vertices.emplace_back(vertex.x, vertex.y, vertex.z);
        }

        // Cargar coordenadas UV
        if (mesh->HasTextureCoords(0)) {
            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
                aiVector3D uv = mesh->mTextureCoords[0][v];
                texCoords.emplace_back(uv.x, uv.y); // Intenta sin invertir
            }
        }
        else {
            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
                texCoords.emplace_back(0.0f, 0.0f);
            }
        }

        // Cargar índices de las caras
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    aiReleaseImport(scene);
    // Inicializa los buffers después de cargar el modelo
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // VBO para vértices
    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // VBO para coordenadas UV
    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
    glEnableVertexAttribArray(1);

    // EBO para índices
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0); // Desvincula el VAO
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

    float camX = sin(camera_angle) * camera_radius;
    float camZ = cos(camera_angle) * camera_radius;

    gluLookAt(camX, camera_height, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    draw_grid();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindVertexArray(vao); // Vincula el VAO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Dibuja usando el EBO
    glBindVertexArray(0); // Desvincula el VAO

    glDisable(GL_TEXTURE_2D);

    camera_angle += 0.005f;
    if (camera_angle > 2.0f * 3.14159265f) camera_angle = 0.0f;
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

    ilInit();
    init_openGL();

    GLuint textureID = LoadTexture("Baker_house.png");
    loadModel("BakerHouse.fbx");

    while (window.processEvents() && window.isOpen()) {
        const auto t0 = hrclock::now();
        display_func(textureID);
        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }

    return 0;
}
