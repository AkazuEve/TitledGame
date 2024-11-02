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

// Texture class
class Texture {
public:
	Texture();
	Texture(const Texture&) = delete;
	~Texture();

	void LoadTextureData(std::string texturePath, GLenum textureSlot);

	void BindTexture();

private:
	GLuint m_texture{ 0 };
	GLenum m_textureSlot{ 0 };

};

// Mesh class
class Mesh {
public:
	// Creates OpenGL buffers
	Mesh();
	Mesh(const Mesh&) = delete;

	~Mesh();

	// Moves data to GPU from existing vectors
	void LoadMeshData(const std::vector<GLfloat>& vBuffer, const std::vector<GLushort>& iBuffer, GLenum indexFormat);

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

// Model class contains mesh data and textures
// After creation data gets rendered when calling Render()
class Model {
public:
	Model();
	~Model();
	Model(const Model&) = delete;

	void AddMesh(const std::vector<GLfloat>& vBuffer, const std::vector<GLushort>& iBuffer, GLenum indexFormat);

	void AddTexture(GLenum textureSlot, std::string textuprePath);

	void BindModel();

	GLushort GetIndexBufferSize() { return m_mesh.GetIndexBufferSize(); }
	GLenum GetIndexBufferFormat() { return m_mesh.GetIndexBufferFormat(); }

	static std::vector<Model*>& GetModelsVector() { return m_models; }


public:
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 0.0f };

	bool isRendered = true;

private:
	Mesh m_mesh;
	std::vector<Texture*> m_textures;

	glm::mat4 m_modelMatrix{ 1.0 };

	static std::vector<Model*> m_models;
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