#include "Render.h"
#include <GL/glew.h>

Render::Render(unsigned short width, unsigned short height, std::shared_ptr<Camera> camera)
    : m_Width(width), m_Height(height), m_Camera(camera), m_ViewportTexture(0) {  // Constructor modificado para recibir la cámara
}

Render::~Render() {
    if (m_ViewportTexture) {
        glDeleteTextures(1, &m_ViewportTexture);
    }
}

void Render::initialize() {
    createViewportTexture();
}

void Render::createViewportTexture() {
    glGenTextures(1, &m_ViewportTexture);
    glBindTexture(GL_TEXTURE_2D, m_ViewportTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Render::updateViewportTexture() {
    glBindTexture(GL_TEXTURE_2D, m_ViewportTexture);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_Width, m_Height, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Render::renderScene(Scene& scene) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_ViewportTexture);
    glViewport(0, 0, m_Width, m_Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Obtener las matrices de vista y proyección de la cámara
    glm::mat4 viewMatrix = m_Camera->getViewMatrix();
    glm::mat4 projectionMatrix = m_Camera->getProjectionMatrix();

    // Aplicamos las matrices a OpenGL
    glLoadIdentity();
    glMultMatrixf(&glm::value_ptr(projectionMatrix)[0]);  // Aplica la matriz de proyección
    glMultMatrixf(&glm::value_ptr(viewMatrix)[0]);  // Aplica la matriz de vista

    scene.Render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    updateViewportTexture();
}

void Render::resizeViewport(unsigned short width, unsigned short height) {
    m_Width = width;
    m_Height = height;
    createViewportTexture();
}

GLuint Render::getViewportTexture() const {
    return m_ViewportTexture;
}
