#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <GL/glew.h>
#include <memory>
#include "Scene.h"
#include "Camera.h"  // A�adido para incluir la c�mara

class Render {
public:
    Render(unsigned short width, unsigned short height, std::shared_ptr<Camera> camera);  // A�adido el puntero a la c�mara
    ~Render();

    void initialize();
    void createViewportTexture();
    void updateViewportTexture();
    void renderScene(Scene& scene);
    void resizeViewport(unsigned short width, unsigned short height);
    GLuint getViewportTexture() const;

    std::shared_ptr<Camera> getCamera() {
        return m_Camera;
    }

    bool m_ViewportFocused = false, m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
    glm::vec2 m_ViewportBounds[2];
    GLuint m_ViewportTexture = 0;
    unsigned short m_Width;
    unsigned short m_Height;

private:
    std::shared_ptr<Camera> m_Camera;  // Puntero a la c�mara
};

#endif // RENDER_H
