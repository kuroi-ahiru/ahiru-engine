#include "Frustum.h"

//actualiza los planos del frustum a partir de la matriz de vista-proyeccion
void Frustum::Update(const glm::mat4& viewProjectionMatrix) {
    //extract los planos del frustum de la matriz
    planes[0].normal = glm::vec3(viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0]); // Left
    planes[0].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];

    planes[1].normal = glm::vec3(viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0]); // Right
    planes[1].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];

    planes[2].normal = glm::vec3(viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1]); // Bottom
    planes[2].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];

    planes[3].normal = glm::vec3(viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1]); // Top
    planes[3].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];

    planes[4].normal = glm::vec3(viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2]); // Near
    planes[4].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2];

    planes[5].normal = glm::vec3(viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2]); // Far
    planes[5].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];

    //normalizar
    for (auto& plane : planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}

//bool Frustum::IsAABBInside(const glm::vec3& min, const glm::vec3& max) const {
//    for (const auto& plane : planes) {
//        glm::vec3 positiveVertex = min;
//        if (plane.normal.x > 0) positiveVertex.x = max.x;
//        if (plane.normal.y > 0) positiveVertex.y = max.y;
//        if (plane.normal.z > 0) positiveVertex.z = max.z;
//
//        if (!plane.IsPointInFront(positiveVertex)) {
//            return false; // AABB está fuera de este plano
//        }
//    }
//    return true; // AABB está dentro de todos los planos
//}
