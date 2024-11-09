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
#include <iostream>

using namespace std;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height) {
    open(title, width, height);
    SDL_Init(SDL_INIT_VIDEO);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Habilitar el docking

    // Si quieres habilitar navegación por teclado:
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

void MyWindow::swapBuffers() const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    static bool show_about = false;

    if (ImGui::BeginMainMenuBar()) {
		//TODO Añadir menús del editor
        if (ImGui::MenuItem("GitHub")) {
        #ifdef _WIN32
                    std::system("start https://github.com/kuroi-ahiru/ahiru-engine");
        #elif __APPLE__
                    std::system("open https://github.com/kuroi-ahiru/ahiru-engine");
        #elif __linux__
                    std::system("xdg-open https://github.com/kuroi-ahiru/ahiru-engine");
        #endif
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