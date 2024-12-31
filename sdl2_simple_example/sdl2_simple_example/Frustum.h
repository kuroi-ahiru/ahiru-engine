#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>

struct Plane {
    glm::vec3 normal;
    float distance;   //desde el origen
        
    bool IsPointInFront(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance >= 0.0f;
    }
};

class Frustum {
public:
    void Update(const glm::mat4& viewProjectionMatrix);
        
    //bool IsAABBInside(const glm::vec3& min, const glm::vec3& max) const;

private:
    Plane planes[6]; // near, far, left, right, top, bottom
};

#endif // FRUSTUM_H

