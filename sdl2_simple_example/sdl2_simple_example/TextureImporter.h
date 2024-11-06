#pragma once
#ifndef TEXTUREIMPORTER_H
#define TEXTUREIMPORTER_H

#include <GL/glew.h>
#include <IL/il.h>
#include <string>

class TextureImporter {
public:
    static GLuint LoadTexture(const char* file);
};

#endif
