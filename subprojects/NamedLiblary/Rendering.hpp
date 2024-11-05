#pragma once

// Standard lib
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#define DEBUGPRINT(x) std::cout << x << std::endl;

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"

// Glm for math
#include "../../ext/source/glm/glm.hpp"
#include "../../ext/source/glm/gtc/matrix_transform.hpp"
#include "../../ext/source/glm/gtc/type_ptr.hpp"

// GLFW for window resize callback
#include "../../ext/precompiled/include/GLFW/glfw3.h"

// UI class to render mroe advanced ui than im capable of
#include "../../src/modules/UI.hpp"

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

// Creates a shader that can be used for rendering objects
class Shader {
public:
	// Creates a shader program from 2 file paths
	// @param absolute or relative path to vertex shader
	// @param absolute or relative path to fragment shader
	Shader(std::string name, std::string vertexFile, std::string fragmentFile);
	Shader(const Shader&) = delete;

	~Shader();

	// Sets the used shader to this one
	void BindShader();

	void RecompileShader();

	// Gets the underlying shader object
	GLuint GetShaderObject() { return m_shaderProgram; }

	static std::vector<Shader*>& GetShadersVector() { return m_shaders; }

	GLuint GetModelUniformLocation() { return m_modelUniform; }
	GLuint GetModelNormalUniformLocation() { return m_modelNormalUniform; }
	GLuint GetCameraUniformLocation() { return m_cameraUniform; }

public:
	std::string name{};

private:
	bool CompileShader(GLuint& shader, std::string& file, GLint shaderType);

private:
	GLuint m_shaderProgram{ 0 };

	GLuint m_modelUniform{ 0 }, m_modelNormalUniform{ 0 }, m_cameraUniform{ 0 };

	std::string m_vertexFile{}, m_fragmentFile{};

	static std::vector<Shader*> m_shaders;
};

class ShaderManager : UIElement {
public:
	ShaderManager();
	~ShaderManager();

	void OnUIRender() override;

private:
	std::vector<Shader*>& m_shaders = Shader::GetShadersVector();
};

// Texture class
class Texture {
public:
	// Creates OpenGL objects
	Texture();
	Texture(const Texture&) = delete;
	~Texture();

	// Sends the data to GPU and sets used texture slot
	void LoadTextureData(std::string texturePath, GLenum textureSlot);

	// Binds the texture
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

// Model class contains mesh data and textures. After creation data gets rendered when calling Render()
class Model {
public:
	Model();
	~Model();
	Model(const Model&) = delete;

	void AddMesh(std::string name, const MeshData& data, GLenum indexFormat);

	void AddTexture(GLenum textureSlot, std::string textuprePath);

	void BindModel();

	GLushort GetIndexBufferSize() { return m_mesh.GetIndexBufferSize(); }
	GLenum GetIndexBufferFormat() { return m_mesh.GetIndexBufferFormat(); }

	static std::vector<Model*>& GetModelsVector() { return m_models; }

public:
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };

	bool isRendered = true;

	std::string name{};

private:
	Mesh m_mesh;
	std::vector<Texture*> m_textures;

	glm::mat4 m_modelMatrix{ 1.0 };
	glm::mat4 m_modelNormalMatrix{ 1.0 };

	static glm::mat4 sm_identityMatrix;

	static std::vector<Model*> m_models;
};

class ModelManager : UIElement {
public:
	ModelManager();
	~ModelManager();

	void OnUIRender() override;

private:
	std::vector<Model*>& m_models = Model::GetModelsVector();
};

// Camera class
class Camera {
public:
	// Create camera instance with name
	Camera(std::string name, bool enabled);
	~Camera();
	Camera(const Camera&) = delete;
	
	// Sends the camera matrix to shader uniform
	void SendDataToShader();

	static std::vector<Camera*>& GetCamerasVector() { return m_cameras; }

public:
	glm::vec3 position{ 0.0f, 0.0f, -5.0f };
	float fov{ 90.0f };
	float nearPlane{ 0.12f };
	float farPlane{ 50.0f };

	std::string name{};

	bool isUsed = false;

private:
	glm::mat4 view{ 1.0f };
	glm::mat4 projection{ 1.0f };

	static std::vector<Camera*> m_cameras;
};

class CameraManager : UIElement {
public:
	CameraManager();

	~CameraManager();

	void OnUIRender() override;

private:
	std::vector<Camera*>& m_cameras = Camera::GetCamerasVector();
	Camera* m_currentCamera = nullptr;
};

// Initializes all rendering stuff including a basic shader and render buffer
void RenderingInit();

// Frees all memory and deletes all OpenGL objects
void RenderingTerminate();

//Render all data stored in mesh classes.
//Requires an existing opengl rendering context.
void Render();