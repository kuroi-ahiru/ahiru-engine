#include "Cone.h"

// Definici�n de los v�rtices del cono
const std::vector<glm::vec3> Cone::vertices = {
    glm::vec3(0.0f, 1.0f, 0.0f),  // V�rtice superior del cono
    glm::vec3(-0.5f, 0.0f, -0.5f), // Base del cono
    glm::vec3(0.5f, 0.0f, -0.5f),
    glm::vec3(0.5f, 0.0f, 0.5f),
    glm::vec3(-0.5f, 0.0f, 0.5f)
};

// Definici�n de las coordenadas de textura (en este caso, simple mapeo cuadrado)
const std::vector<glm::vec2> Cone::texCoords = {
    glm::vec2(0.5f, 1.0f),  // V�rtice superior
    glm::vec2(0.0f, 0.0f),  // Base
    glm::vec2(1.0f, 0.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 1.0f)
};

// Definici�n de los �ndices para formar los tri�ngulos
const std::vector<unsigned int> Cone::indices = {
    0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,  // Tri�ngulos laterales
    1, 2, 3, 3, 4, 1  // Base del cono (cerramos la base con tri�ngulos)
};
