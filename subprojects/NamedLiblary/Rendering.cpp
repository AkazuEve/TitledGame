#include "Rendering.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../ext/source/stb/stbi.h"

#pragma region PREDEF
static MeshData LoadModelFromPLYFile(std::string filePath);
static std::string LoadShaderFile(std::string file);
static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
static void RenderedWindowResizeCallback(GLFWwindow* window, int width, int height);
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
Shader* defaultShader = nullptr;
Shader* normalDebugShader = nullptr;
Shader* positionDebugShader = nullptr;

Shader* currentShader = nullptr;

Camera* currentCamera;

ShaderManager* shaderManager = nullptr;

CameraManager* cameraManager = nullptr;

ModelManager* modelmanager = nullptr;

// Default clear color
glm::vec3 defaultClearColor = { 0.2f, 0.2f, 0.6f };

// Render buffer stuff
unsigned int renderWidth{ 1440 }, renderHeight{ 810 };

GLuint renderBufferColorTexture{ 0 };

#pragma endregion

#pragma region Shader

std::vector<Shader*> Shader::m_shaders;

Shader::Shader(std::string name, std::string vertexFile, std::string fragmentFile) {
	m_shaders.push_back(this);
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

			// Delete temporary shader objects
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			m_modelUniform = glGetUniformLocation(m_shaderProgram, "model");
			m_modelNormalUniform = glGetUniformLocation(m_shaderProgram, "modelNormal");
			m_cameraUniform = glGetUniformLocation(m_shaderProgram, "camera");

			DEBUGPRINT("Created shader: " << m_shaderProgram);
	}
}

Shader::~Shader() {
	// Remove this pointer from models vector
	std::vector<Shader*>::iterator position = std::find(m_shaders.begin(), m_shaders.end(), this);
	if (position != m_shaders.end()) {
		m_shaders.erase(position);
		DEBUGPRINT("Removed Shader from shader list: " << this);
	}

	DEBUGPRINT("Destroyed shader: " << m_shaderProgram);
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
		return false;
	}

	return true;
}
#pragma endregion

#pragma region ShaderManager 

ShaderManager::ShaderManager() {

}

ShaderManager::~ShaderManager() {

}

void ShaderManager::OnUIRender() {
	ImGui::Begin("Shaders");

	// Cant forget this
	//ImGui::Image(reinterpret_cast<ImTextureID>(m_curentBuffer), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

	for (Shader* shader : m_shaders) {
		if (ImGui::TreeNode(shader->name.c_str())) {
			if (ImGui::Button("Recompile")) {
				shader->RecompileShader();
			}
			if (ImGui::Button("Set as default")) {
				currentShader = shader;
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}

#pragma endregion

#pragma region Texture

Texture::Texture() {
	// Generate texture object
	//glGenTextures(1, &m_texture);
	glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);

	// Assigns the texture to a Texture Unit
	//glBindTexture(GL_TEXTURE_2D, m_texture);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	//glBindTexture(GL_TEXTURE_2D, 0);

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
	//glActiveTexture(m_textureSlot);
	//glBindTexture(GL_TEXTURE_2D, m_texture);

	// Send data to GPU
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glTextureStorage2D(m_texture, 5, GL_RGBA8, x, y);
	glTextureSubImage2D(m_texture, 0, 0, 0, x, y, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

	// Generate mipmaps
	//glGenerateMipmap(GL_TEXTURE_2D);

	// Free memory cause were good peope
	stbi_image_free(bytes);

	// Bind texture 0 no funny happens
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::BindTexture() {
	//glActiveTexture(m_textureSlot);
	//glBindTexture(GL_TEXTURE_2D, m_texture);

	glBindTextureUnit(m_textureSlot - GL_TEXTURE0, m_texture);
}

#pragma endregion

#pragma region Mesh

Mesh::Mesh() {
	// Create OpenGL array and buffers
	//glGenVertexArrays(1, &m_vertexArray);
	//glGenBuffers(1, &m_vertexBuffer);
	//glGenBuffers(1, &m_indexBuffer);

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

#pragma endregion

#pragma region Model

glm::mat4 Model::sm_identityMatrix{ 1.0f };
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

void Model::AddMesh(std::string name, const MeshData& data, GLenum indexFormat) {
	this->name = name;
	m_mesh.LoadMeshData(data, indexFormat);
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

	static glm::mat4 translationMatrix{ 1.0f };
	static glm::mat4 rotationMatrix{ 1.0f };
	static glm::mat4 scaleMatrix{ 1.0f };

	translationMatrix = glm::translate(sm_identityMatrix, position);

	rotationMatrix = glm::rotate(sm_identityMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	scaleMatrix = glm::scale(sm_identityMatrix, scale);

	m_modelNormalMatrix = translationMatrix * rotationMatrix;
	m_modelMatrix = m_modelNormalMatrix * scaleMatrix;

	// Send model matrix to shader
	glUniformMatrix4fv(currentShader->GetModelNormalUniformLocation(), 1, GL_FALSE, glm::value_ptr(m_modelNormalMatrix));
	glUniformMatrix4fv(currentShader->GetModelUniformLocation(), 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

	// Bind mesh and all its textures
	m_mesh.BindMesh();
	for (Texture* texture : m_textures) {
		texture->BindTexture();
	}
}

#pragma endregion

#pragma region ModelManager

ModelManager::ModelManager() {
	CreateModel("Default", "res/models/untitled.ply", "res/textures/brick.png", GL_UNSIGNED_SHORT);
}

ModelManager::~ModelManager() {
	for (Model* model : m_models) {
		delete(model);
	}
}

void ModelManager::CreateModel(std::string name, std::string modelPath, std::string texturePath, GLenum indexFormat) {
	Model* tmpPtr = new Model;

	MeshData data = LoadModelFromPLYFile(modelPath);

	tmpPtr->AddMesh(name, data, indexFormat);
	tmpPtr->AddTexture(GL_TEXTURE0, texturePath);
}

void ModelManager::OnUIRender() {
	ImGui::Begin("Model Manager");

	static char name[20]{ "Test" };
	static char textureName[20]{ "brick" };
	static char modelName[20]{ "untitled" };
	ImGui::InputText("Name", name, 20);
	ImGui::InputText("Texture Name", textureName, 20);
	ImGui::InputText("Model Name", modelName, 20);
	if (ImGui::Button("Create Object")) {
		std::string texturePath = std::string("res/textures/") + textureName + std::string(".png");
		std::string modelPath = std::string("res/models/") + modelName + std::string(".ply");

		CreateModel(name, modelPath, texturePath, GL_UNSIGNED_SHORT);
	}

	ImGui::Separator();

	if (m_models.size() > 0) {

		unsigned int i = 0;
		for (Model* model : m_models) {
			if (ImGui::TreeNode(std::string(model->name + "##" + std::to_string(i)).c_str())) {
				i++;
				ImGui::Checkbox("Is Rendered", &model->isRendered);
				ImGui::DragFloat3("Position", (float*)&model->position, 0.05f, -10, 10);
				ImGui::DragFloat3("Rotation", (float*)&model->rotation, 0.5f, -360, 360);
				ImGui::DragFloat3("Scale", (float*)&model->scale, 0.05f, 0, 5);

				if (ImGui::Button("Remove")) {
					DEBUGPRINT("Removed Model from Object manager list: " << model);
					delete(model);
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset Transform")) {
					model->position = glm::vec3(0.0f);
					model->rotation = glm::vec3(0.0f);
					model->scale = glm::vec3(1.0f);
				}

				ImGui::TreePop();
			}
		}


	}
	ImGui::End();
}
#pragma endregion

#pragma region Camera

std::vector<Camera*> Camera::m_camers;

Camera::Camera(std::string name, bool enabled) {
	this->name = name;
	this->isUsed = enabled;
	
	DEBUGPRINT("Added " << name << " Camera to camera pointer list: " << this);
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
	glUniformMatrix4fv(currentShader->GetCameraUniformLocation(), 1, GL_FALSE, glm::value_ptr(result));
}

#pragma endregion

#pragma region CameraManager

CameraManager::CameraManager() {
	m_cameras.push_back(new Camera("Default", true));
	currentCamera = m_cameras[0];
}

CameraManager::~CameraManager() {
	for (Camera* camera : m_cameras) {
		delete(camera);
	}
}

void CameraManager::CreateCamera(std::string name, bool enabled) {
	m_cameras.push_back(new Camera(name, enabled));
}


void CameraManager::OnUIRender() {
	ImGui::Begin("Camera Manager");

	static char newCameraName[20]{ "Not Default" };

	ImGui::InputText("New Camera Name", newCameraName, 20);

	if (ImGui::Button("Create new camera")) {
		m_cameras.push_back(new Camera(newCameraName, true));
	}

	ImGui::Separator();

	for (Camera* camera : m_cameras) {

		if (ImGui::TreeNode(camera->name.c_str())) {
			ImGui::DragFloat3("Position", (float*)&camera->position, 0.05f, -10, 10);
			ImGui::DragFloat("Fov", (float*)&camera->fov, 0.01f, 20.0f, 180.0f);
			ImGui::DragFloat("Near Plane", (float*)&camera->nearPlane, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat("Far Plane", (float*)&camera->farPlane, 0.01f, 20.0f, 100.0f);

			if (ImGui::Button("Use")) {
				currentCamera = camera;
			}

			ImGui::SameLine();

			if (m_cameras.size() > 1) {
				if (ImGui::Button("Delete")) {
					// Remove this pointer from camera pointer vector
					std::vector<Camera*>::iterator position = std::find(m_cameras.begin(), m_cameras.end(), camera);
					if (position != m_cameras.end()) {
						m_cameras.erase(position);
						DEBUGPRINT("Removed Camera from model poiter vector: " << this);

						currentCamera = m_cameras[0];
						delete(camera);
					}
				}
				ImGui::SameLine();
			}
			if (ImGui::Button("Reset Transform")) {
				camera->position = glm::vec3(0.0f, 0.0f, -5.0f);
			}

			ImGui::TreePop();
		}
	}
	ImGui::End();
}

#pragma endregion

void RenderingInit() {
	// Load all OpenGL functions
	gladLoadGL();

	UI::ImGuiInit("#version 460");

	// Turns out depth buffer wont work when its not enabled :>
	glEnable(GL_DEPTH_TEST);

	glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, 1.0f);

	// Setup resize callback so were always up to date with render resolution
	glfwSetWindowSizeCallback(glfwGetCurrentContext(), RenderedWindowResizeCallback);

	currentShader = defaultShader = new Shader("Default", "res/shaders/default.vert", "res/shaders/default.frag");
	normalDebugShader = new Shader("Normal Debug", "res/shaders/default.vert", "res/shaders/normalDebug.frag");
	positionDebugShader = new Shader("Position Debug", "res/shaders/default.vert", "res/shaders/positionDebug.frag");

	shaderManager = new ShaderManager;

	modelmanager = new ModelManager;

	Camera::GetCameraVector().reserve(4);
	cameraManager = new CameraManager;

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
	UI::ImGuiTerminate();

	delete(defaultShader);
	delete(normalDebugShader);
	delete(positionDebugShader);

	delete(shaderManager);

	delete(modelmanager);

	delete(cameraManager);
}

void CreateCamera(std::string name, bool enabled) {
	cameraManager->CreateCamera(name, enabled);
}

void Render() {
	// Bind the render buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set rendering resolution
	glViewport(0, 0, renderWidth, renderHeight);

	if(currentShader) currentShader->BindShader();
	
	if(currentCamera) currentCamera->SendDataToShader();

	// Render data form all instances of Model if enabled
	for (Model* model : Model::GetModelsVector()) {
		if (model->isRendered) {
			model->BindModel();
			glDrawElements(GL_TRIANGLES, model->GetIndexBufferSize(), model->GetIndexBufferFormat(), 0);
		}
	}

	UI::RenderUI();
}

#pragma region HelperFunctions

static MeshData LoadModelFromPLYFile(std::string filePath) {
	std::ifstream file{ filePath };
	if (!file.is_open())
		throw std::exception("Failed to open file");

	DEBUGPRINT("Loading ply file: " << filePath);

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
				DEBUGPRINT("Vertex count: " << vertexCount);
			}

			if (word == "face") {
				stream >> triangleCount;
				data.indices.reserve(3 * triangleCount);
				DEBUGPRINT("Triangle count: " << triangleCount);
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
static void RenderedWindowResizeCallback(GLFWwindow* window, int width, int height) {
	renderWidth = width;
	renderHeight = height;

	DEBUGPRINT("New window size width: " << renderWidth << " height: " << renderHeight);
}

#pragma endregion