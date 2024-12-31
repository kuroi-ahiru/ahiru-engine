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

float camera_angle = 0.0f; // angulo d camara
const float camera_radius = 10.0f; // distancia de la camara al origen

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
bool streetLoaded = false;
bool rotation = false;

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;

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

        xOffset = mouseX - lastMouseX;
        yOffset = lastMouseY - mouseY;//Invertimos la y

        xOffset *= sensitivity;
        yOffset *= sensitivity;

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 newcameraDirection;
        newcameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newcameraDirection.y = sin(glm::radians(pitch));
        newcameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(newcameraDirection);

        if (state[SDL_SCANCODE_W]) {

            cameraPos += cameraSpeed * cameraFront;
            if (state[SDL_SCANCODE_LSHIFT]) {

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
        if (state[SDL_SCANCODE_LALT]) {

            if (rotation == false)
            {
                std::cout << "Camera rotation enabled" << std::endl;
                rotation = true;
			}
            else
            {
                std::cout << "Camera rotation stopped" << std::endl;
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
        std::cout << "Camera centered" << std::endl;
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

    projectionMatrix = glm::perspective(glm::radians(fov), (float)WINDOW_SIZE.x / (float)WINDOW_SIZE.y, 0.1f, 100.0f); //quizas mover de lugar

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

    projectionMatrix = glm::perspective(glm::radians(fov), (float)WINDOW_SIZE.x / (float)WINDOW_SIZE.y, 0.1f, 100.0f); //mousepicking

    while (window.processEvents() && window.isOpen()) {

        const auto t0 = hrclock::now();
        if (!window.IsPaused()) {
            updateCamera();
            scene.Update();
        }
        viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);//mousepicking

        // Actualiza los planos del frustum
        scene.CalculateFrustumPlanes(projectionMatrix, viewMatrix);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
            cameraPos.x + cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
            cameraUp.x, cameraUp.y, cameraUp.z);

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

                glm::vec3 rayDir = CalculateRay(mouseX, mouseY, WINDOW_SIZE.x, WINDOW_SIZE.y, projectionMatrix, viewMatrix);
                glm::vec3 rayOrigin = cameraPos;

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
 /*       if (!streetLoaded)
        {
            auto bakerHouse = scene.CreateGameObject("street.fbx", 0);
            scene.AddGameObject(bakerHouse);
			streetLoaded = true;
        }*/

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