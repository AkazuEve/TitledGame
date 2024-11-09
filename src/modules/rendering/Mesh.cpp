#include "Mesh.hpp"

Mesh::Mesh() {
	// Create OpenGL array and buffers
	glCreateVertexArrays(1, &m_vertexArray);
	glCreateBuffers(1, &m_vertexBuffer);
	glCreateBuffers(1, &m_indexBuffer);

	DEBUGPRINT("Created mesh: " << m_vertexArray);
}

Mesh::~Mesh() {
	// Destroy OpenGL objects
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
	glDeleteVertexArrays(1, &m_vertexArray);

	DEBUGPRINT("Destroyed mesh: " << m_vertexArray);
}

void Mesh::LoadMeshData(const MeshData& data, GLenum indexFormat) {
	// Send data to buffers
	glNamedBufferStorage(m_vertexBuffer, data.vertices.size() * sizeof(Vertex), data.vertices.data(), 0);
	glNamedBufferStorage(m_indexBuffer, data.indices.size() * sizeof(GLshort), data.indices.data(), 0);

	glVertexArrayVertexBuffer(m_vertexArray, 0, m_vertexBuffer, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(m_vertexArray, m_indexBuffer);

	// Set buffer layout
	glEnableVertexArrayAttrib(m_vertexArray, 0);
	glVertexArrayAttribFormat(m_vertexArray, 0, 3, GL_FLOAT, GL_FALSE, (GLuint)(offsetof(Vertex, pos)));
	glVertexArrayAttribBinding(m_vertexArray, 0, 0);

	glEnableVertexArrayAttrib(m_vertexArray, 1);
	glVertexArrayAttribFormat(m_vertexArray, 1, 3, GL_FLOAT, GL_FALSE, (GLuint)(offsetof(Vertex, nor)));
	glVertexArrayAttribBinding(m_vertexArray, 1, 0);

	glEnableVertexArrayAttrib(m_vertexArray, 2);
	glVertexArrayAttribFormat(m_vertexArray, 2, 2, GL_FLOAT, GL_FALSE, (GLuint)(offsetof(Vertex, tex)));
	glVertexArrayAttribBinding(m_vertexArray, 2, 0);

	m_indexBufferSize = data.indices.size();
	m_indexBufferFormat = indexFormat;
}

void Mesh::BindMesh() {
	glBindVertexArray(m_vertexArray);
}