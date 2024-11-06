#include "TextureImporter.h"
#include <iostream>
#include <string>

// Función auxiliar para convertir de char* a wchar_t* usando mbstowcs_s
std::wstring ConvertToWideChar(const char* file) {
    size_t len = strlen(file) + 1;
    std::wstring wfile(len, L'#');
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, &wfile[0], len, file, _TRUNCATE);
    return wfile;
}

GLuint TextureImporter::LoadTexture(const char* file) {
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    // Convertimos la ruta del archivo a wchar_t*
    std::wstring wfile = ConvertToWideChar(file);

    if (!ilLoadImage(wfile.c_str())) {
        std::cerr << "Error al cargar la textura: " << file << std::endl;
        ilDeleteImages(1, &imageID);
        return 0;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
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


