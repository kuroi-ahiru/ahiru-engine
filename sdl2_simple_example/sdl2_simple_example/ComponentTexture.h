#ifndef COMPONENTTEXTURE_H
#define COMPONENTTEXTURE_H

#include "Component.h"
#include <GL/glew.h>
#include <string>

class ComponentTexture : public Component {
public:
    ComponentTexture(GameObject* owner, GLuint textureID, int width, int height, const std::string& filePath)
        : Component(Type::Texture, owner), textureID(textureID), width(width), height(height), filePath(filePath), useCheckers(false) {}

    void Update() override {}
    void Render() override {
        if (useCheckers) {
            // Código para aplicar la textura en "checkers"
            glBindTexture(GL_TEXTURE_2D, checkerTextureID);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }
    }

    // Info para el inspector
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    const std::string& GetFilePath() const { return filePath; }

    // Activar o desactivar la textura de "checkers"
    void SetUseCheckers(bool enable) { useCheckers = enable; }

    // Método para cargar una textura de "checkers" (patrón de cuadros)
    void LoadCheckerTexture() {
        glGenTextures(1, &checkerTextureID);
        glBindTexture(GL_TEXTURE_2D, checkerTextureID);
        unsigned char checkerData[64 * 64 * 3];  // Patrón 8x8 de 64x64 píxeles

        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 64; x++) {
                int index = (y * 64 + x) * 3;
                bool isWhite = (x / 8 + y / 8) % 2 == 0;
                checkerData[index] = isWhite ? 255 : 0;
                checkerData[index + 1] = isWhite ? 255 : 0;
                checkerData[index + 2] = isWhite ? 255 : 0;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, checkerData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

private:
    GLuint textureID;            // ID de la textura principal
    GLuint checkerTextureID;     // ID de la textura de cuadros (checkers)
    int width, height;           // Dimensiones de la textura
    std::string filePath;        // Camino de la textura
    bool useCheckers;            // Bandera para activar o desactivar "checkers"
};

#endif // COMPONENTTEXTURE_H

