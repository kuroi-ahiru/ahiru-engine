#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <glm/glm.hpp>

class Sphere {
public:
    static std::vector<glm::vec3> GenerateVertices(float radius, unsigned int slices, unsigned int stacks);
    static std::vector<glm::vec2> GenerateTexCoords(unsigned int slices, unsigned int stacks);
    static std::vector<unsigned int> GenerateIndices(unsigned int slices, unsigned int stacks);

    static const std::vector<glm::vec3> vertices;
    static const std::vector<glm::vec2> texCoords;
    static const std::vector<unsigned int> indices;
};

#endif // SPHERE_H
