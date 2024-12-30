#include "Cube.h"

// Definición de los 8 vértices del cubo
const std::vector<glm::vec3> Cube::vertices = {
    glm::vec3(-0.5f, -0.5f, -0.5f), // V0
    glm::vec3(0.5f, -0.5f, -0.5f), // V1
    glm::vec3(0.5f,  0.5f, -0.5f), // V2
    glm::vec3(-0.5f,  0.5f, -0.5f), // V3
    glm::vec3(-0.5f, -0.5f,  0.5f), // V4
    glm::vec3(0.5f, -0.5f,  0.5f), // V5
    glm::vec3(0.5f,  0.5f,  0.5f), // V6
    glm::vec3(-0.5f,  0.5f,  0.5f)  // V7
};

// Definición de los índices para formar 12 triángulos (2 por cara)
const std::vector<unsigned int> Cube::indices = {
    0, 2, 1, 0, 3, 2,  // Cara frontal
    4, 5, 6, 4, 6, 7,  // Cara trasera
    0, 1, 5, 0, 5, 4,  // Cara inferior
    2, 3, 7, 2, 7, 6,  // Cara superior
    0, 4, 7, 0, 7, 3,  // Cara izquierda
    1, 2, 6, 1, 6, 5   // Cara derecha
};

// Definición de coordenadas de textura por defecto
const std::vector<glm::vec2> Cube::texCoords = {
    // Cara frontal
    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
    // Cara trasera
    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
    // Cara inferior
    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
    // Cara superior
    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
    // Cara izquierda
    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
    // Cara derecha
    glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)
};
