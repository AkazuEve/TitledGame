#pragma once


// Standard lib
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#define DEBUGPRINT(x) std::cout << x << std::endl;

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"
#include "../../ext/source/glm/glm.hpp"

#include "../../ext/precompiled/include/GLFW/glfw3.h"

// Creates a shader that can be used for rendering objects
class Shader {
public:
	// Creates a shader program from 2 file paths
	// @param absolute or relative path to vertex shader
	// @param absolute or relative path to fragment shader
	Shader(std::string vertexFile, std::string fragmentFile);
	Shader(const Shader&) = delete;

	~Shader();

	// Sets the used shader to this one
	void Bind();

	static Shader* currentShader;

private:
	void CompileShader(GLuint& shader, std::string& file, GLint shaderType);
private:
	GLuint m_shaderProgram = 0;
};

// Mesh class
// After creation data gets rendered when calling Render()
class Mesh {
public:
	// Creates OpenGL buffers and moves data to GPU from existing vectors
	Mesh(const std::vector<GLfloat>& vBuffer, const std::vector<GLushort>& iBuffer);
	Mesh(const Mesh&) = delete;

	~Mesh();

	void Bind();

	// Returns size of the initially passed in index vector 
	GLushort GetIndexBufferSize() { return m_indexBufferSize; }
	bool* GetEnabled() { return m_isEnabled; }

	static std::vector<Mesh*> renderableMeshes;

private:
	GLuint m_vertexArray{ 0 };
	GLuint m_vertexBuffer{ 0 };
	GLuint m_indexBuffer{ 0 };

	GLushort m_indexBufferSize{ 0 };

	bool* m_isEnabled{ nullptr };
};

// Initializes all rendering stuff including a basic shader and render buffer
void RenderingInit();

// Frees all memory and deletes all OpenGL objects
void RenderingTerminate();

// Get the id of currently used gBuffer color texture
GLuint GetRenderBufferColorTexture();

// Get the id of currently used gBuffer normal texture
GLuint GetRenderBufferNormalTexture();

// Get the id of currently used gBuffer depth texture
GLuint GetRenderBufferDepthTexture();

//Render all data stored in mesh classes.
//Requires an existing opengl rendering context.
void Render();