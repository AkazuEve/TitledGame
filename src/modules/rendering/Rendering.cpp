#include "Rendering.hpp"

#pragma region PREDEF
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

#pragma region ModelManager

ModelManager::ModelManager() {
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

void Rendering::Init() {
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

	Shader::GetShadersVector().reserve(5);
	shaderManager = new ShaderManager;

	Model::GetModelsVector().reserve(10);
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

void Rendering::Terminate() {
	UI::ImGuiTerminate();

	delete(defaultShader);
	delete(normalDebugShader);
	delete(positionDebugShader);

	delete(shaderManager);

	delete(modelmanager);

	delete(cameraManager);
}

void Rendering::CreateCamera(std::string name, bool enabled) {
	cameraManager->CreateCamera(name, enabled);
}

void Rendering::CreateModel(std::string name, std::string modelPath, std::string texturePath, GLenum indexFormat) {
	modelmanager->CreateModel(name, modelPath, texturePath, indexFormat);
}

void Rendering::Render() {
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