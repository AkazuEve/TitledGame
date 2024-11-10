#pragma once

#include "../../ext/source/glm/glm.hpp"

struct Vertex {
	Vertex(glm::vec3 postition, glm::vec3 normal, glm::vec2 textureCoordinate);
	Vertex();

	glm::vec3 pos{};
	glm::vec3 nor{};
	glm::vec2 tex{};
};