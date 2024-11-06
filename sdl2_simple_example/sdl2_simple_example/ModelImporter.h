#pragma once

#ifndef MODELIMPORTER_H
#define MODELIMPORTER_H

#include <vector>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

using namespace std;

class ModelImporter {

public:
	bool LoadModel(const char* file);
	const std::vector<glm::vec3>& GetVertices() const { return vertices; }
	const std::vector<glm::vec2>& GetTexCoords() const { return texCoords; }
	const std::vector<unsigned int>& GetIndices() const { return indices; }

private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int> indices;
};


#endif