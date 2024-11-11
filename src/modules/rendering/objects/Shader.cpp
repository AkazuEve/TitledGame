#include "Shader.hpp"

#include "../../resources/ResourceManager.hpp"

Shader::Shader(std::string name, std::string vertexFile, std::string fragmentFile) {
	m_shaderProgram = glCreateProgram();

	this->name = name;

	m_vertexFile = vertexFile;
	m_fragmentFile = fragmentFile;

	// Create temporary shader objects
	GLuint vertexShader{ 0 }, fragmentShader{ 0 };

	// Read and compile shader source code
	if (CompileShader(vertexShader, m_vertexFile, GL_VERTEX_SHADER) && CompileShader(fragmentShader, m_fragmentFile, GL_FRAGMENT_SHADER)) {
		// Attach shaders to program
		glAttachShader(m_shaderProgram, vertexShader);
		glAttachShader(m_shaderProgram, fragmentShader);

		// Link program
		glLinkProgram(m_shaderProgram);

		// Detach no longer needed shaders
		glDetachShader(m_shaderProgram, vertexShader);
		glDetachShader(m_shaderProgram, fragmentShader);

		// Delete temporary shader objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Get uniform locations for quicker access later
		m_modelUniform = glGetUniformLocation(m_shaderProgram, "model");
		m_modelNormalUniform = glGetUniformLocation(m_shaderProgram, "modelNormal");
		m_cameraUniform = glGetUniformLocation(m_shaderProgram, "camera");
	}
}

Shader::~Shader() {
	glDeleteProgram(m_shaderProgram);
}

void Shader::BindShader() {
	glUseProgram(m_shaderProgram);
}

void Shader::RecompileShader() {
	GLuint vertexShader{ 0 }, fragmentShader{ 0 };

	// Read and compile shader source code
	if (CompileShader(vertexShader, m_vertexFile, GL_VERTEX_SHADER) && CompileShader(fragmentShader, m_fragmentFile, GL_FRAGMENT_SHADER)) {
		GLuint shaderProgram{ 0 };

		shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		// Link program
		glLinkProgram(shaderProgram);

		// Delete temporary shader objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glDeleteProgram(m_shaderProgram);

		m_shaderProgram = shaderProgram;

		DEBUGPRINT("Shader " << name << " recompiled");
	}
}

bool Shader::CompileShader(GLuint& shader, std::string& file, GLint shaderType) {

	// Create shader object with passed in variable
	shader = glCreateShader(shaderType);

	// Read source from file and compile
	std::string shaderSource;
	const char* vertexSourceCharPtr;
	shaderSource = ResourceManager::LoadShader(file);
	vertexSourceCharPtr = shaderSource.c_str();
	glShaderSource(shader, 1, &vertexSourceCharPtr, NULL);
	glCompileShader(shader);

	// Check if compiling shader has failed and print out errors in console
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		GLchar* message = (GLchar*)malloc(length * sizeof(GLchar));
		glGetShaderInfoLog(shader, length, &length, message);

		DEBUGPRINT("Failed to compile " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment"));
		DEBUGPRINT(message);

		free(message);
		return false;
	}

	return true;
}