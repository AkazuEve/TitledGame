#pragma once

#include <vector>
#include <string>

#include "../../debug/Debug.hpp"

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"

#include "../../resources/ResourceManager.hpp"

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
};