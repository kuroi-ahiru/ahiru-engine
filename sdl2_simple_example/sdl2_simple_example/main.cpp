#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <memory> // Para std::shared_ptr
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "imgui_impl_sdl2.h"
#include <IL/il.h>
#include <GL/glu.h> // para la perspectiva de glu
#include <glm/gtc/matrix_transform.hpp> 

#include "Scene.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "Camera.h"

#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//funcion aux para calcular rayo mouse
glm::vec3 CalculateRay(int mouseX, int mouseY, int screenWidth, int screenHeight,
    const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight; // Invertir Y
    float z = 1.0f;

    glm::vec4 rayClip(x, y, -1.0f, 1.0f); // Coordenadas en espacio de recorte
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye.z = -1.0f; // En direccion del ojo
    rayEye.w = 0.0f;

    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    return glm::normalize(rayWorld); // Normalizar el rayo
}


using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
static const ivec2 WINDOW_SIZE(1200, 600); //1300 800 para poder trabajr en el portatil luego se cambia
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

bool houseLoaded = false;
bool rotation = false;

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
    Scene scene;

    ilInit();
    init_openGL();

    //instancia de Camera
    Camera camera(glm::vec3(0.0f, 5.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f,
        (float)WINDOW_SIZE.x / (float)WINDOW_SIZE.y, 0.1f, 100.0f);

    while (window.processEvents() && window.isOpen()) {

        const auto t0 = hrclock::now();
        if (!window.IsPaused()) {
            //actualizar camara
            const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
            int mouseX, mouseY;
            Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
            camera.Update(FRAME_DT.count(), keyboardState, mouseX, mouseY, mouseState);
                        
            scene.Update();
        }
        //renderizar
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z,
                  camera.GetPosition().x + camera.GetViewMatrix()[2][0],
                  camera.GetPosition().y + camera.GetViewMatrix()[2][1],
                  camera.GetPosition().z + camera.GetViewMatrix()[2][2],
                  0.0f, 1.0f, 0.0f);

        scene.DrawGrid();
        scene.Render();

        std::shared_ptr<GameObject> currentSelectedObject = scene.GetSelectedGameObject();

        window.display_func(currentSelectedObject, scene);
        window.swapBuffers();

        //mouse picking no funciona por el bug q hay con el docking de la ImGui
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            if (!ImGui::GetIO().WantCaptureMouse) { // asegurarse de que WantCaptureMouse es correcto
                std::cout << "Mouse picking triggered." << std::endl;
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                glm::vec3 rayDir = camera.CalculateRay(mouseX, mouseY, WINDOW_SIZE.x, WINDOW_SIZE.y);
                glm::vec3 rayOrigin = camera.GetPosition();

                auto selectedObject = scene.PickGameObject(rayOrigin, rayDir);

                if (selectedObject) {
                    printf("GameObject seleccionado: %s\n", selectedObject->GetName().c_str());
                }
                else {
                    printf("No se seleccionó ningún GameObject\n");
                }
            }
            else {
                std::cout << "Mouse interaction blocked by ImGui.\n";
            }
        }


        //Dejo esta funcion de game object picking funcional sin tener el cuenta el bug de la ImGui no borrar porfis
        
        //// Picking de GameObjects
        //if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        //    int mouseX, mouseY;
        //    SDL_GetMouseState(&mouseX, &mouseY);

        //    glm::vec3 rayDir = CalculateRay(mouseX, mouseY, WINDOW_SIZE.x, WINDOW_SIZE.y, projectionMatrix, viewMatrix);
        //    glm::vec3 rayOrigin = cameraPos; // Usamos la pos de la camara como origen del rayo

        //    auto selectedObject = scene.PickGameObject(rayOrigin, rayDir);

        //    if (selectedObject) {
        //        printf("GameObject seleccionado: %s\n", selectedObject->GetName().c_str());
        //    }
        //    else {
        //        printf("No se seleccionó ningún GameObject\n");
        //    }
        //}


        // Carga automatica de la casica esa con la textura al arrancar el motor
   //     if (!houseLoaded)
   //     {
   //         auto bakerHouse = scene.CreateGameObject("BakerHouse.fbx", "Baker_house.png");
   //         scene.AddGameObject(bakerHouse);
			//houseLoaded = true;
   //     }

        // DRAG AND DROP + crear GameObject del objeto dropeado
        std::string droppedFile = window.getDroppedFile();
        if (!droppedFile.empty() && droppedFile.find(".fbx") != std::string::npos) {

            auto dropped = scene.CreateGameObject(droppedFile.c_str(), "Baker_house.png");

            if (dropped != NULL) {
                scene.AddGameObject(dropped);
            }
        }

        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) std::this_thread::sleep_for(FRAME_DT - dt);
    }

    return 0;
}