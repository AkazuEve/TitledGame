#pragma once

#include "../../ext/source/glm/glm.hpp"

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"

#include <vector>

struct Vertex {
	Vertex(glm::vec3 postition, glm::vec3 normal, glm::vec2 textureCoordinate) : pos(postition), nor(normal), tex(textureCoordinate) { }
	Vertex() {}
	glm::vec3 pos{};
	glm::vec3 nor{};
	glm::vec2 tex{};
};

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;

	GLenum indexFormat{ 0 };
};