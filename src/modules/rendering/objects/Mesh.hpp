#pragma once

#include "../../Debug.hpp"

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"

#include "../structs/MeshData.hpp"

class Mesh {
public:
	// Creates OpenGL buffers
	Mesh();
	Mesh(const Mesh&) = delete;

	~Mesh();

	// Moves data to GPU from existing vectors
	void LoadMeshData(const MeshData& data, GLenum indexFormat);

	void BindMesh();

	// Returns size of the initially passed in index vector 
	GLushort GetIndexBufferSize() { return m_indexBufferSize; }
	GLushort GetIndexBufferFormat() { return m_indexBufferFormat; }

private:
	GLuint m_vertexArray{ 0 };
	GLuint m_vertexBuffer{ 0 };
	GLuint m_indexBuffer{ 0 };

	GLushort m_indexBufferSize{ 0 };
	GLenum m_indexBufferFormat{ 0 };

	bool* m_isEnabled{ nullptr };
};