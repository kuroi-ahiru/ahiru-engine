#include "Sphere.h"
#include <cmath>

const float PI = 3.14159265358979323846f;  // Definici�n de Pi

// La generaci�n de v�rtices para la esfera, utilizando latitudes (stacks) y longitudes (slices)
std::vector<glm::vec3> Sphere::GenerateVertices(float radius, unsigned int slices, unsigned int stacks) {
    std::vector<glm::vec3> vertices;

    // Generamos los v�rtices para el cuerpo de la esfera
    for (unsigned int i = 0; i <= stacks; ++i) {
        float phi = PI * float(i) / float(stacks);  // �ngulo polar (latitud)
        for (unsigned int j = 0; j <= slices; ++j) {
            float theta = 2.0f * PI * float(j) / float(slices);  // �ngulo azimutal (longitud)

            // C�lculo de las coordenadas esf�ricas
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    return vertices;
}

// Generaci�n de coordenadas de textura
std::vector<glm::vec2> Sphere::GenerateTexCoords(unsigned int slices, unsigned int stacks) {
    std::vector<glm::vec2> texCoords;

    // Se mapean las coordenadas UV de forma proporcional
    for (unsigned int i = 0; i <= stacks; ++i) {
        for (unsigned int j = 0; j <= slices; ++j) {
            float u = float(j) / float(slices);
            float v = float(i) / float(stacks);

            texCoords.push_back(glm::vec2(u, v));
        }
    }

    return texCoords;
}

// Generaci�n de �ndices para los tri�ngulos
std::vector<unsigned int> Sphere::GenerateIndices(unsigned int slices, unsigned int stacks) {
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < slices; ++j) {
            unsigned int first = i * (slices + 1) + j;
            unsigned int second = (i + 1) * (slices + 1) + j;
            unsigned int third = (i + 1) * (slices + 1) + (j + 1);
            unsigned int fourth = i * (slices + 1) + (j + 1);

            // Dos tri�ngulos por cada cu�druple de v�rtices
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(third);

            indices.push_back(first);
            indices.push_back(third);
            indices.push_back(fourth);
        }
    }

    return indices;
}

// Asignaci�n de los valores est�ticos de la esfera
const std::vector<glm::vec3> Sphere::vertices = Sphere::GenerateVertices(1.0f, 36, 18);  // Radio de 1, 36 slices y 18 stacks
const std::vector<glm::vec2> Sphere::texCoords = Sphere::GenerateTexCoords(36, 18);
const std::vector<unsigned int> Sphere::indices = Sphere::GenerateIndices(36, 18);
