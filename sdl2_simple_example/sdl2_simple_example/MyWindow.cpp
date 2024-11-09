#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "SDL2/SDL.h"
#include <cstdlib>
#include <windows.h>
#include <psapi.h>
#include <iostream>
using namespace std;

Uint32 lastTime = 0;
Uint32 currentTime;
float fps = 0.0f;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) {
    open(title, width, height);
    SDL_Init(SDL_INIT_VIDEO);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Habilitar el docking

    // Si quieres habilitar navegaci�n por teclado:
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");
}

MyWindow::~MyWindow() {
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    close();
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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    static bool show_about = false;
    static bool show_performance = false;

    if (ImGui::BeginMainMenuBar()) {
		//TODO A�adir men�s del editor
        if (ImGui::MenuItem("Performance")) {
            show_performance = !show_performance;
        }
        if (ImGui::MenuItem("GitHub")) {
        std::system("start https://github.com/kuroi-ahiru/ahiru-engine");
        }
        if (ImGui::MenuItem("About")) {
            show_about = true;
        }
        if (ImGui::MenuItem("Quit")) {
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
        }
        ImGui::EndMainMenuBar();
    }

    if (show_about) {
        ImGui::OpenPopup("About");
        show_about = false;
    }

    //Ventana de performance
    if (show_performance) {
        ImGui::Begin("Performance", &show_performance, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

        //Calcular FPS manualmente
        currentTime = SDL_GetTicks();
        fps = 1000.0f / (currentTime - lastTime);
        lastTime = currentTime;
        ImGui::Text("FPS: %.1f", fps);

        //Uso de memoria real
        size_t memory_mb = getMemoryUsage() / 1024; //KB a MB
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

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
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
            droppedFile = file; // Almacena el archivo soltado en droppedFile
            SDL_free(file);     // Libera la memoria
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