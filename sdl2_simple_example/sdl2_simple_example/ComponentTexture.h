#ifndef COMPONENTTEXTURE_H
#define COMPONENTTEXTURE_H

#include "Component.h"
#include <GL/glew.h>
#include <string>

class ComponentTexture : public Component {
public:

    GLuint textureID;  
    
    ComponentTexture(GameObject* owner, GLuint textureID, int width, int height, const std::string& filePath)
        : Component(Type::Texture, owner), textureID(textureID), width(width), height(height), filePath(filePath), useCheckers(false) {}

    void Update() override {}
    void Render() override {
        if (useCheckers) {
        
            glBindTexture(GL_TEXTURE_2D, checkerTextureID);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }
    }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    const std::string& GetFilePath() const { return filePath; }

    bool IsUsingCheckers() const { return useCheckers; }
    void SetUseCheckers(bool enable) { useCheckers = enable; }

    // intento para cargar textura de los cuadritos del workshop de clase
    void LoadCheckerTexture() {
        glGenTextures(1, &checkerTextureID);
        glBindTexture(GL_TEXTURE_2D, checkerTextureID);
        unsigned char checkerData[64 * 64 * 3];  

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
    GLuint checkerTextureID;   
    int width, height;         
    std::string filePath;      
    bool useCheckers;          
};

#endif // COMPONENTTEXTURE_H

