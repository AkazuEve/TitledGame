#include "Rendering.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../ext/source/stb/stbi.h"

#pragma region PREDEF
static std::string LoadShaderFile(std::string file);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void RenderedWindowResizeCallback(GLFWwindow* window, int width, int height);

struct Vertex {
	glm::vec3 pos;
	glm::vec3 nor;
	glm::vec2 tex;
};

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

#pragma endregion

#pragma region Const Variables
static std::vector<GLfloat> quadVertices ={
//     COORDINATES     / Texture cordinates
	-1.0f, -1.0f, 
	-1.0f,  1.0f, 
	 1.0f,  1.0f, 
	 1.0f, -1.0f, 
};

// Indices for vertices order
static std::vector<GLushort> quadIndices = {
	0, 2, 1, // Upper triangle
	0, 3, 2 // Lower triangle
};

#pragma endregion

#pragma region Variables

// Store them as pointers cause they have to be created after initialization of OpenGL
Shader* prePass = nullptr;
Shader* firstPas = nullptr;

// Default clear color
glm::vec3 defaultClearColor = { 0.2f, 0.2f, 0.6f };

// gBuffer stuff
GLuint gBuffer{ 0 }, gColorTexture{ 0 }, gNormalTexture{ 0 }, gDepthTexture{ 0 };
unsigned int renderWidth{ 1440 }, renderHeight{ 810 };

// Shader uniforms shortcuts
GLuint cameraUniform{ 0 }, modelUniform{ 0 };

// Render quad stuff
GLuint renderQuadVBO{ 0 }, renderQuadvBuffer{ 0 }, renderQuadiBuffer{ 0 };

#pragma endregion

#pragma region Shader

Shader::Shader(std::string vertexFile, std::string fragmentFile) {
	m_shaderProgram = glCreateProgram();

	// Create temporary shader objects
	GLuint vertexShader, fragmentShader;

	// Read and compile shader source code
	CompileShader(vertexShader, vertexFile, GL_VERTEX_SHADER);
	CompileShader(fragmentShader, fragmentFile, GL_FRAGMENT_SHADER);

	// Attach shaders to program
	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);

	// Link program
	glLinkProgram(m_shaderProgram);

	// Delete temporary shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	DEBUGPRINT("Created shader: " << m_shaderProgram);
}

Shader::~Shader() {
	DEBUGPRINT("Destroyed shader: " << m_shaderProgram);
	glDeleteProgram(m_shaderProgram);
}

void Shader::BindShader() {
	glUseProgram(m_shaderProgram);
}

void Shader::CompileShader(GLuint& shader, std::string& file, GLint shaderType) {

	// Create shader object with passed in variable
	shader = glCreateShader(shaderType);

	// Read source from file and compile
	std::string shaderSource;
	const char* vertexSourceCharPtr;
	shaderSource = LoadShaderFile(file);
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
	}
}
#pragma endregion

#pragma region Texture

Texture::Texture() {
	// Generate texture object
	glGenTextures(1, &m_texture);

	// Assigns the texture to a Texture Unit
	glBindTexture(GL_TEXTURE_2D, m_texture);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);

	DEBUGPRINT("Created texture: " << m_texture);
}

Texture::~Texture() {
	DEBUGPRINT("Destroyed texture: " << m_texture);
	glDeleteTextures(1, &m_texture);
}

void Texture::LoadTextureData(std::string texturePath, GLenum textureSlot) {
	m_textureSlot = textureSlot;

	int x, y, n;
	// Flip horizontally so OpenGL images are not flipped
	stbi_set_flip_vertically_on_load(true);

	// Read data from file
	unsigned char* bytes = stbi_load(texturePath.c_str(), &x, &y, &n, 0);

	// Bind texture object
	glActiveTexture(m_textureSlot);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	// Send data to GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Free memory cause were good peope
	stbi_image_free(bytes);

	// Bind texture 0 no funny happens
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::BindTexture() {
	glActiveTexture(m_textureSlot);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

#pragma endregion

#pragma region Mesh

Mesh::Mesh() {
	// Create OpenGL array and buffers
	glGenVertexArrays(1, &m_vertexArray);
	glGenBuffers(1, &m_vertexBuffer);
	glGenBuffers(1, &m_indexBuffer);
	
	DEBUGPRINT("Created mesh: " << m_vertexArray);
}

Mesh::~Mesh() {
	// Destroy OpenGL objects
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
	glDeleteVertexArrays(1, &m_vertexArray);

	DEBUGPRINT("Destroyed mesh: " << m_vertexArray);
}

void Mesh::LoadMeshData(const std::vector<GLfloat>& vBuffer, const std::vector<GLushort>& iBuffer, GLenum indexFormat) {
	// Bind buffers 
	glBindVertexArray(m_vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	// Send data to buffers
	glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iBuffer.size() * sizeof(GLshort), iBuffer.data(), GL_STATIC_DRAW);

	// Set buffer layout
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind stuff so no explosions happen
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	m_indexBufferSize = iBuffer.size();
	m_indexBufferFormat = indexFormat;
}

void Mesh::BindMesh() {
	glBindVertexArray(m_vertexArray);
}

#pragma endregion

#pragma region Model

std::vector<Model*> Model::m_models;

Model::Model() {
	// Push created model pointer to vector
	m_models.push_back(this);
	DEBUGPRINT("Created model: " << this);
}

Model::~Model() {
	// Destroy all allocated textures
	for (Texture* texture : m_textures) {
		delete(texture);
	}

	// Remove this pointer from models vector
	std::vector<Model*>::iterator position = std::find(m_models.begin(), m_models.end(), this);
	if (position != m_models.end()) {
		m_models.erase(position);
		DEBUGPRINT("Removed Model from model list: " << this);
	}
}

void Model::AddMesh(std::string name, const std::vector<GLfloat>& vBuffer, const std::vector<GLushort>& iBuffer, GLenum indexFormat) {
	this->name = name;
	m_mesh.LoadMeshData(vBuffer, iBuffer, indexFormat);
}

void Model::AddTexture(GLenum textureSlot, std::string textuprePath) {
	// Create new texture
	Texture* tmp = new Texture();

	// Setup data and push to texture vector
	tmp->LoadTextureData(textuprePath, textureSlot);
	m_textures.push_back(tmp);

}

void Model::BindModel() {
	// Recalculate new matrix
	m_modelMatrix = glm::mat4{ 1.0f };

	m_modelMatrix = glm::translate(m_modelMatrix, position);

	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	m_modelMatrix = glm::scale(m_modelMatrix, scale);
	// Send model matrix to shader
	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

	// Bind mesh and all its textures
	m_mesh.BindMesh();
	for (Texture* texture : m_textures) {
		texture->BindTexture();
	}
}

#pragma endregion

#pragma region Camera

std::vector<Camera*> Camera::m_cameras{};

Camera::Camera(std::string name, bool enabled) {
	this->name = name;
	this->isUsed = enabled;
	// Push this pointer to cameras vector
	m_cameras.push_back(this);
	DEBUGPRINT("Added " << name << " Camera to camera list: " << this);
}

Camera::~Camera() {
	// Remove this pointer from cameras vector
	std::vector<Camera*>::iterator position = std::find(m_cameras.begin(), m_cameras.end(), this);
	if (position != m_cameras.end()) {
		m_cameras.erase(position);
		DEBUGPRINT("Removed Camera " << name << "from camera list: " << this);
	}
}

void Camera::SendDataToShader() {
	// Reset 
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	// Calculate view and projection matrix
	view = glm::translate(view, position);
	projection = glm::perspective(glm::radians(fov), (float)(renderWidth / renderHeight), nearPlane, farPlane);

	static glm::mat4 result{ 0.0f };
	result = projection * view;
	glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, glm::value_ptr(result));
}

#pragma endregion

void RenderingInit() {
	// Load all OpenGL functions
	gladLoadGL();

	// Turns out depth buffer wont work when its not enabled :>
	glEnable(GL_DEPTH_TEST);

	// Setup resize callback so were always up to date with render resolution
	glfwSetWindowSizeCallback(glfwGetCurrentContext(), RenderedWindowResizeCallback);

	// Create gBuffer for rendering
	{
		// Generate frame buffer
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// Create frame buffer color texture
		glGenTextures(1, &gColorTexture);
		glBindTexture(GL_TEXTURE_2D, gColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gColorTexture, 0);

		// Create frame buffer color texture 2
		glGenTextures(1, &gNormalTexture);
		glBindTexture(GL_TEXTURE_2D, gNormalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalTexture, 0);

		// Create frame buffer depth texture
		glGenTextures(1, &gDepthTexture);
		glBindTexture(GL_TEXTURE_2D, gDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, renderWidth, renderHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepthTexture, 0);

		// Attach textures to frame buffer
		GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT};
		glDrawBuffers(2, attachments);
	}

	// Create quad for rendering
	{
		// Generate and bind vertex array object
		glGenVertexArrays(1, &renderQuadVBO);
		glBindVertexArray(renderQuadVBO);

		// Generate vertex and index buffers
		glGenBuffers(1, &renderQuadvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, renderQuadvBuffer);

		glGenBuffers(1, &renderQuadiBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderQuadiBuffer);

		// Send vertex and index buffer data to gpu
		glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(GLfloat), quadVertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quadIndices.size() * sizeof(GLshort), quadIndices.data(), GL_STATIC_DRAW);

		// Set vertex attributes
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		
		// Unbind all objects so no boo boo happens
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Create the render pipeline shaders
	prePass = new Shader("res/shaders/pre.vert", "res/shaders/pre.frag");
	firstPas = new Shader("res/shaders/first.vert", "res/shaders/first.frag");

	modelUniform = glGetUniformLocation(prePass->GetShaderObject(), "model");
	cameraUniform = glGetUniformLocation(prePass->GetShaderObject(), "camera");

	// Chack for debug flags from GLFW and enable debug in OpenGL if needed
	GLint flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}

void RenderingTerminate() {
	// Free memory cause am a good person
	delete(prePass);
	delete(firstPas);

	// Delete all OpenGL objects
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteTextures(1, &gColorTexture);
	glDeleteTextures(1, &gNormalTexture);

	glDeleteVertexArrays(1, &renderQuadVBO);
	glDeleteBuffers(1, &renderQuadvBuffer);
	glDeleteBuffers(1, &renderQuadiBuffer);
}

GLuint GetRenderBufferColorTexture() {
	return gColorTexture;
}

GLuint GetRenderBufferNormalTexture() {
	return gNormalTexture;
}

GLuint GetRenderBufferDepthTexture() {
	return gDepthTexture;
}

void Render() {
	// Run render prepass
	{
		// Bind the gBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// Clear color and depth buffer
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set rendering resolution
		glViewport(0, 0, renderWidth, renderHeight);

		glBindTexture(GL_TEXTURE_2D, gColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, gNormalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, gDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, renderWidth, renderHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);

		// Set pre pass shader as active
		prePass->BindShader();

		// Render data form all instances of Model if enabled
		for (Camera* camera : Camera::GetCamerasVector()) {
			if (camera->isUsed) {
				camera->SendDataToShader();
			}
		}

		for (Model* model : Model::GetModelsVector()) {
			if (model->isRendered) {
				model->BindModel();
				glDrawElements(GL_TRIANGLES, model->GetIndexBufferSize(), model->GetIndexBufferFormat(), 0);
			}
		}
	}

	// Run first pass
	{
		// Bind default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind a whole screen square
		glBindVertexArray(renderQuadVBO);

		// Setup gBuffer textures so thier data can be used to render the final image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gColorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormalTexture);

		// Bind the first pass shader
		firstPas->BindShader();

		// Render the final image
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		// Unbind all gBuffer textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

#pragma region HelperFunctions

static MeshData LoadModelFromPLYFile(std::string& filePath) {
	std::ifstream file{ filePath };
	if (!file.is_open())
		throw std::exception("Failed to open file");

	MeshData data;
	Vertex veretx;
	unsigned int tmpInt;

	std::string line;
	std::string word;
	std::stringstream stream;

	unsigned int vertexCount = 0;
	unsigned int triangleCount = 0;


	while (std::getline(file, line)) {
		stream.clear();
		stream.str(line);

		stream >> word;

		//Here I read how many vertices and indices the model has
		//For now I don't care for the layout just want it to work
		if (word == "element") {

			stream >> word;

			if (word == "vertex") {
				stream >> vertexCount;
				data.vertices.reserve(vertexCount);
			}

			if (word == "face") {
				stream >> triangleCount;
				data.indices.reserve(3 * triangleCount);
			}
		}

		//End of header time for data parsing
		if (word == "end_header") {
			//This just contains vertex data
			for (unsigned int i{ 0 }; i < vertexCount; i++) {
				std::getline(file, line);
				stream.clear();
				stream.str(line);

				stream >> veretx.pos.x >> veretx.pos.y >> veretx.pos.z;
				stream >> veretx.nor.x >> veretx.nor.y >> veretx.nor.z;
				stream >> veretx.tex.x >> veretx.tex.y;

				data.vertices.emplace_back(veretx);

			}

			//This line will have a int for vectex amount per face then data >:|
			for (unsigned int i{ 0 }; i < triangleCount; i++) {
				std::getline(file, line);
				stream.clear();
				stream.str(line);

				stream >> tmpInt;

				for (unsigned int i{ 0 }; i < 3; i++) {
					stream >> tmpInt;
					data.indices.emplace_back(tmpInt);
				}

			}
		}
	}
	return data;
}

static std::string LoadShaderFile(std::string file) {
	std::string shaderString;
	std::ifstream inFile(file.c_str(), std::ios::ate);

	//Just open text and dunk into string
	if (inFile.is_open()) {
		shaderString.reserve(inFile.tellg());
		inFile.seekg(0, std::ios::beg);
		shaderString.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
	}
	else {
		throw std::runtime_error("Failed to load shader file");
	}

	return shaderString;
}

static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam) {
	// ignore non-significant error/warning codes
	//if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

// Set render resolution to window resolusion when the size changes
void RenderedWindowResizeCallback(GLFWwindow* window, int width, int height) {
	renderWidth = width;
	renderHeight = height;

	DEBUGPRINT("New window size width: " << renderWidth << " height: " << renderHeight);
}

#pragma endregion