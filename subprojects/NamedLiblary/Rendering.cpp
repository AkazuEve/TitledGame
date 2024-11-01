#include "Rendering.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../ext/source/stb/stbi.h"

#pragma region PREDEF
static std::string LoadShaderFile(std::string file);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void RenderedWindowResizeCallback(GLFWwindow* window, int width, int height);
#pragma endregion

#pragma region ConstVars
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

struct Vertex {
	glm::vec3 pos;
	glm::vec3 nor;
	glm::vec2 tex;
};
struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

#pragma region Shader

Shader* Shader::currentShader = nullptr;

Shader::Shader(std::string vertexFile, std::string fragmentFile) {
	m_shaderProgram = glCreateProgram();

	
	GLuint vertexShader, fragmentShader;

	CompileShader(vertexShader, vertexFile, GL_VERTEX_SHADER);
	CompileShader(fragmentShader, fragmentFile, GL_FRAGMENT_SHADER);

	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);

	glLinkProgram(m_shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(m_shaderProgram);
	currentShader = this;

	DEBUGPRINT("Created shader");
}

Shader::~Shader() {
	DEBUGPRINT("Destroyed shader");
	glDeleteProgram(m_shaderProgram);
}

void Shader::Bind() {
	glUseProgram(m_shaderProgram);
	currentShader = this;
}

void Shader::CompileShader(GLuint& shader, std::string& file, GLint shaderType) {

	shader = glCreateShader(shaderType);

	std::string shaderSource;
	const char* vertexSourceCharPtr;
	shaderSource = LoadShaderFile(file);
	vertexSourceCharPtr = shaderSource.c_str();
	glShaderSource(shader, 1, &vertexSourceCharPtr, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		GLchar* message = (GLchar*)malloc(length * sizeof(GLchar));
		glGetShaderInfoLog(shader, length, &length, message);

		std::cout << "Failed to compile " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;

		free(message);
	}
}
#pragma endregion

#pragma region Texture

class Texture {
public:
	Texture(GLenum slot) {
		glGenTextures(1, &m_texture);
		// Assigns the texture to a Texture Unit
		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		// Configures the type of algorithm that is used to make the image smaller or bigger
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Configures the way the texture repeats (if it does at all)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		int x, y, n;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);
		// Reads the image from a file and stores it in bytes
		unsigned char* bytes = stbi_load("res/textures/brick.png", &x, &y, &n, 0);

		// Assigns the image to the OpenGL Texture object
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
		// Generates MipMaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Deletes the image data as it is already in the OpenGL Texture object
		stbi_image_free(bytes);

		// Unbinds the OpenGL Texture object so that it can't accidentally be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	~Texture() {
		glDeleteTextures(1, &m_texture);
	}

	void Bind() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}

private:
	GLuint m_texture{ 0 };
};

#pragma endregion

#pragma region Mesh

std::vector<Mesh*> Mesh::renderableMeshes{};

Mesh::Mesh(const std::vector<GLfloat>& vBuffer, const std::vector<GLushort>& iBuffer) {

	glGenVertexArrays(1, &m_vertexArray);
	glBindVertexArray(m_vertexArray);

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iBuffer.size() * sizeof(GLshort), iBuffer.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_indexBufferSize = iBuffer.size();
	renderableMeshes.push_back(this);

	DEBUGPRINT("Created mesh");
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
	glDeleteVertexArrays(1, &m_vertexArray);

	DEBUGPRINT("Destroyed mesh");
}

void Mesh::Bind() {
	glBindVertexArray(m_vertexArray);
}

#pragma endregion

class Object {
public:
	Object() {

	}
	~Object() {

	}

	void Bind() {

	}

private:
	std::vector<Texture> m_textures;
	//Mesh m_mesh;

};

// Store them as pointers cause they have to be created after initialization of OpenGL
Shader* prePass = nullptr;
Shader* firstPas = nullptr;

Texture* popcat;

// Default clear color
glm::vec3 defaultClearColor = { 0.2f, 0.2f, 0.6f };
 
// gBuffer stuff
GLuint gBuffer{ 0 }, gColorTexture{ 0 }, gNormalTexture{ 0 }, gDepthTexture{ 0 };
unsigned int renderWidth{ 1440 }, renderHeight{ 810 };

// Render quad stuff
GLuint renderQuadVBO{ 0 }, renderQuadvBuffer{ 0 }, renderQuadiBuffer{ 0 };

void RenderingInit() {
	// Load all OpenGL functions
	gladLoadGL();

	glEnable(GL_DEPTH_TEST);

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

	// Create the render pipeline shader
	prePass = new Shader("res/shaders/pre.vert", "res/shaders/pre.frag");
	firstPas = new Shader("res/shaders/first.vert", "res/shaders/first.frag");

	popcat = new Texture(GL_TEXTURE0);

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

	delete(popcat);

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

		// Set pre pass shader as active
		prePass->Bind();

		// Set rendering resolution
		glViewport(0, 0, renderWidth, renderHeight);

		glBindTexture(GL_TEXTURE_2D, gColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, gNormalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, gDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, renderWidth, renderHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);

		popcat->Bind();

		// Render data form all instances of mesh if enabled
		for (Mesh* mesh : Mesh::renderableMeshes) {
			mesh->Bind();
			glDrawElements(GL_TRIANGLES, mesh->GetIndexBufferSize(), GL_UNSIGNED_SHORT, 0);
		}
	}

	// Run first pass
	{
		// Bind default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind the first pass shader
		firstPas->Bind();

		// Bind a whole screen square
		glBindVertexArray(renderQuadVBO);

		// Setup gBuffer textures so thier data can be used to render the final image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gColorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormalTexture);

		// Render the final image
		glDrawElements(GL_TRIANGLES, quadIndices.size(), GL_UNSIGNED_SHORT, 0);

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