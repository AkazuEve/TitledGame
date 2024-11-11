#pragma once

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"

#include <vector>

#include "Vertex.hpp"

struct MeshData {
	std::vector<Vertex> vertices{};
	std::vector<GLuint> indices{};
};