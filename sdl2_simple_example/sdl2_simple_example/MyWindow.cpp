#include <GL/glew.h> //pongo aquyi para test error
#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include <memory>               // Para std::shared_ptr
#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "SDL2/SDL.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentTexture.h"
#include "Scene.h" //nose si hace falta revisar includes
#include <cstdlib>
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include "imgui_internal.h"
#include <cmath>
#include <algorithm>
#include <streambuf>
#include "Cube.h"

using namespace std;

ImGuiIO* g_io = nullptr;

Uint32 lastTime = 0;
Uint32 currentTime;
float fps = 0.0f;
static std::vector<std::string> console_log;
static char console_input[256] = "";

GLuint playIconTexture = 0;
GLuint pauseIconTexture = 0;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) : console_buffer(console_log) {

    open(title, width, height);
    setIcon("patonegro.bmp");
    original_cout_buffer = std::cout.rdbuf(&console_buffer);
    SDL_Init(SDL_INIT_VIDEO);
    ImGui::CreateContext();

    g_io = &ImGui::GetIO();
    g_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
    g_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // A�adir en el json docking-experimental
 
    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");

    createViewportTexture();
}

MyWindow::~MyWindow() {

    if (original_cout_buffer) {
        std::cout.rdbuf(original_cout_buffer);
    }

    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    close();
    SDL_Quit();
}

void MyWindow::open(const char* title, unsigned short width, unsigned short height) {

    if (isOpen()) return;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!_window) throw exception(SDL_GetError());

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw exception(SDL_GetError());
    if (SDL_GL_MakeCurrent(_window, _ctx) != 0) throw exception(SDL_GetError());
    if (SDL_GL_SetSwapInterval(1) != 0) throw exception(SDL_GetError());
}

void MyWindow::close() {

    if (!isOpen()) return;

    SDL_GL_DeleteContext(_ctx);
    _ctx = nullptr;

    SDL_DestroyWindow(static_cast<SDL_Window*>(_window));
    _window = nullptr;
}

size_t getMemoryUsage() {

    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {

        return pmc.PagefileUsage / 1024; //bytes a KB
    }
    return 0;
}

void MyWindow::swapBuffers() const {

    SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
}

void MyWindow::createViewportTexture() {
    glGenTextures(1, &m_ViewportTexture);
    glBindTexture(GL_TEXTURE_2D, m_ViewportTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MyWindow::updateViewportTexture() {
    glBindTexture(GL_TEXTURE_2D, m_ViewportTexture);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, _width, _height, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MyWindow::display_func(std::shared_ptr<GameObject> selectedObject, Scene& scene) {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    updateViewportTexture();

    static bool show_about = false;
    static bool show_help = false;
    static bool show_performance = false;
    float menuHeight = 0.0f;
    // Menú de clic derecho

    // TODO SI DA TIEMPO: QUE SOLO SE ABRA ESTE MENU EN LA HIERARCHY

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup("RightClickMenu");
    }

    // Crear el menú de clic derecho
    if (ImGui::BeginPopup("RightClickMenu")) {
        if (ImGui::BeginMenu("Add Primitive")) {
            // Las opciones de las primitivas (Cube, Sphere, Cone) deben ir aquí
            if (ImGui::MenuItem("Cube")) {
                // Lógica para agregar un cubo a la escena
                scene.AddCube("Cube", glm::vec3(2.0f, 0.0f, 0.0f));
            }
            if (ImGui::MenuItem("Sphere")) {
                // Lógica para agregar una esfera a la escena
                scene.AddSphere("Sphere", glm::vec3(2.0f, 0.0f, 0.0f));
            }
            if (ImGui::MenuItem("Cone")) {
                // Lógica para agregar un cono a la escena
                scene.AddCone("Cone", glm::vec3(2.0f, 0.0f, 0.0f));
            }
            ImGui::EndMenu(); // Cierra el submenú Add Primitive
        }

        ImGui::EndPopup(); // Cierra el popup RightClickMenu
    }


    if (ImGui::BeginMainMenuBar()) {

        menuHeight = ImGui::GetWindowSize().y;

        if (ImGui::MenuItem("Performance")) {
            show_performance = !show_performance;
        }
        if (ImGui::MenuItem("GitHub")) {
            std::system("start https://github.com/kuroi-ahiru/ahiru-engine");
        }
        if (ImGui::MenuItem("About")) {
            show_about = true;
        }
        if (ImGui::MenuItem("Open ImGui Help")) {
            show_help = true;
        }
        if (ImGui::MenuItem("Close ImGui Help")) {
            show_help = false;
        }
        //checkbox debug rayo mouse picking
        static bool debugRayEnabled = false;
        if (ImGui::Checkbox("Debug Ray Mode", &debugRayEnabled)) {
            scene.SetDebugMode(debugRayEnabled); //activar/desactivar modo debug en Scene
            std::cout << "Debug Ray Mode: " << (debugRayEnabled ? "Enabled" : "Disabled") << std::endl; //para comorobar si va
        } 
        if (ImGui::MenuItem("Quit")) {
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
        }        
        ImGui::EndMainMenuBar();
    }

    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - menuHeight));
    ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove  );

    ImGui::DockSpace(ImGui::GetID("MainDockSpace"));
    ImGui::End();

    if (show_performance) {

        ImGui::Begin("Performance", &show_performance, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

        currentTime = SDL_GetTicks();
        fps = 1000.0f / (currentTime - lastTime);
        lastTime = currentTime;
        ImGui::Text("FPS: %.1f", fps);

        size_t memory_mb = getMemoryUsage() / 1024;
        ImGui::Text("Memory Usage: %zu MB", memory_mb);

        static float fps_history[100] = {};
        static int fps_index = 0;
        fps_history[fps_index] = fps;
        fps_index = (fps_index + 1) % 100;

        static float memory_history[100] = {};
        static int memory_index = 0;
        memory_history[memory_index] = static_cast<float>(memory_mb);
        memory_index = (memory_index + 1) % 100;

        ImGui::PlotLines("FPS", fps_history, IM_ARRAYSIZE(fps_history), 0, nullptr, 0.0f, 120.0f, ImVec2(0, 80));
        ImGui::PlotLines("Memory Usage (KB)", memory_history, IM_ARRAYSIZE(memory_history), 0, nullptr, 0.0f, 300.0f, ImVec2(0, 80));

        ImGui::End();
    }

    if (show_help)
    {
        ImGui::ShowDemoWindow();
    }

    if (ImGui::Begin("Inspector")) {

        ImGui::Text("Inspector");
        ImGui::Separator();
           
        if (selectedObject) {

            ImGui::Text("Selected GameObject: %s", selectedObject->GetName().c_str());

            for (auto& component : selectedObject->GetComponents()) {

                switch (component->GetType()) {

                    case Component::Type::Transform: {

                        auto* transform = dynamic_cast<ComponentTransform*>(component.get());
                        if (transform) {
                            /*static float position[3] = { transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z };
                            static float rotation[3] = { transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z };
                            static float scale[3] = { transform->GetScale().x, transform->GetScale().y, transform->GetScale().z };*/

                            ImGui::Separator();
                            ImGui::Text("Transform Component");

                            /*ImGui::InputFloat3("Position: ", position);
                            ImGui::InputFloat3("Rotation: ", rotation);
                            ImGui::InputFloat3("Scale: ", scale);*/

                            // Editable Position
                            glm::vec3 position = transform->GetPosition();
                            if (ImGui::InputFloat3("Position", &position[0])) {
                                transform->SetPosition(position);
                            }

                            // Editable Rotation
                            glm::vec3 rotation = transform->GetRotation();
                            if (ImGui::InputFloat3("Rotation", &rotation[0])) {
                                transform->SetRotation(rotation);
                            }

                            // Editable Scale
                            glm::vec3 scale = transform->GetScale();
                            if (ImGui::InputFloat3("Scale", &scale[0])) {
                                transform->SetScale(scale);
                            }
                        }
                        break;
                    }
                    case Component::Type::Mesh: {
                        auto* mesh = dynamic_cast<ComponentMesh*>(component.get());

                        if (mesh) {
                            ImGui::Separator();
                            ImGui::Text("Mesh Component");

							// Tick apra mostrar u ocultar el modelo
                            bool isVisible = mesh->IsVisible();
                            if (ImGui::Checkbox("Set Active", &isVisible)) {
                                mesh->SetVisible(isVisible);  // Actualiza el estado de visibilidad
                            }

                            // Tick para las normales
                            bool showNormals = mesh->IsShowNormalsEnabled();
                            if (ImGui::Checkbox("Show Normals", &showNormals)) {
                                mesh->SetShowNormals(showNormals);
                            }
                        }
                        break;
                    }
                case Component::Type::Texture: {

                    auto* texture = dynamic_cast<ComponentTexture*>(component.get());

                    if (texture) {
                        ImGui::Separator();
                        ImGui::Text("Texture Component");
                        ImGui::Text("Size: %dx%d", texture->GetWidth(), texture->GetHeight());
                        ImGui::Text("Path: %s", texture->GetFilePath().c_str());
                        bool useCheckers = texture->IsUsingCheckers();
                        if (ImGui::Checkbox("Use Checker Texture", &useCheckers)) {
                            texture->SetUseCheckers(useCheckers);
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
        else {

            ImGui::Text("No GameObject selected");
        }
        ImGui::End();
    }
    if (ImGui::Begin("Hierarchy)")) {

        ImGui::Text("Hierarchy");
        ImGui::Separator();
        ImGui::Text("GameObjects in Scene");

        int index = 0;
        for (const auto& gameObject : scene.GetGameObjects()) {

            std::string label = "GameObject " + std::to_string(index++);

            if (ImGui::Selectable(label.c_str(), gameObject == scene.GetSelectedGameObject())) {
                scene.SetSelectedGameObject(gameObject);
            }
        }
        ImGui::End();
    }
    if  (ImGui::Begin("Console")) {

        ImGui::Text("Console");
        ImGui::Separator();

        for (const auto& line : console_log) {

            ImGui::TextUnformatted(line.c_str());
        }

        if (ImGui::InputText("##ConsoleInput", console_input, IM_ARRAYSIZE(console_input), ImGuiInputTextFlags_EnterReturnsTrue)) {

            console_log.push_back(std::string("> ") + console_input);

            if (std::string(console_input) == "clear") {

                console_log.clear();
            }
            else if (std::string(console_input) == "fps") {

                console_log.push_back("Current FPS: " + std::to_string(fps));
            }
            else if (std::string(console_input) == "memory") {

                console_log.push_back("Memory usage: " + std::to_string(getMemoryUsage() / 1024) + " MB");
            }
            else if (std::string(console_input) == "help") {

                console_log.push_back("Commands:\n   - fps: Show the current fps\n   - memory: Shows the memory usage\n   - quit: Close the engine");
                console_log.push_back("Controls:\n   - Right click + WASD: Moves the camera\n   - Right click + Q or E: ZoomIn or ZoomOut\n   - F: Centers the camera\n   - Left click + Left Alt: Rotates the camera");
            }
            else if (std::string(console_input) == "quit") {

                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            else {

                console_log.push_back("Unrecognized command: " + std::string(console_input));
            }

            strcpy_s(console_input, "");
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {

            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::End();
    }

    if (show_about) {

        ImGui::OpenPopup("About");
        show_about = false;
    }

    if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text("Ahiru Engine: \n");
        ImGui::Text("Version: 0.1.0");
        ImGui::Text("Developer: Kuroi Ahiru");
        ImGui::Text("Libraries Used: SDL2, OpenGL, ImGui, Devil, Assimp");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {

            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::Begin("Game Control")) {
        if (ImGui::ImageButton((void*)(intptr_t)(isPaused ? playIconTexture : pauseIconTexture), 
            ImVec2(32, 32))) {
            isPaused = !isPaused;
        }
        ImGui::SameLine();
        ImGui::Text(isPaused ? "Resume" : "Pause");
    }
    ImGui::End();

    // VIEWPORT -------------------
    ImGui::Begin("Viewport");
    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

    uint32_t textureID = m_ViewportTexture;
    ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, { 0, 1 }, { 1, 0 });

    ImGui::End();
    // END VIEWPORT ---------------

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //swapBuffers();
}


bool MyWindow::processEvents(IEventProcessor* event_processor) {

    SDL_Event e;
    while (SDL_PollEvent(&e)) {

        ImGui_ImplSDL2_ProcessEvent(&e);
        if (event_processor) event_processor->processEvent(e);

        switch (e.type) {

            case SDL_QUIT:
                close();
                return false;

            case SDL_DROPFILE: {

                char* file = e.drop.file;
                droppedFile = file; // gurda el archivo soltado en droppedFile
                SDL_free(file);     
                break;
            }
        }
    }
    return true;
}

std::string MyWindow::getDroppedFile() {

    std::string file = droppedFile;
    droppedFile.clear();
    return file;
}

void MyWindow::setIcon(const char* iconPath) {
    SDL_Surface* icon = SDL_LoadBMP("patonegro.bmp");
    if (!icon) {
        std::cerr << "Error loading icon BMP: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_SetWindowIcon(_window, icon);
    SDL_FreeSurface(icon);
}
    
void MyWindow::LoadIcons(Scene& scene) {
    playIconTexture = scene.LoadTexture("play.png");
    pauseIconTexture = scene.LoadTexture("pause.png");
}