#include "Rendering.hpp"

#pragma region PREDEF
static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
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
Camera* currentCamera = nullptr;

ShaderManager* shaderManager = nullptr;
CameraManager* cameraManager = nullptr;
ModelManager* modelmanager = nullptr;

// Default clear color
glm::vec3 defaultClearColor = { 0.2f, 0.2f, 0.6f };

// Render buffer stuff
unsigned int renderWidth{ 1440 }, renderHeight{ 810 };

#pragma endregion

void Rendering::Init() {
	// Load all OpenGL functions
	gladLoadGL();

	UI::ImGuiInit("#version 460");

	// Turns out depth buffer wont work when its not enabled :>
	glEnable(GL_DEPTH_TEST);

	glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, 1.0f);

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
		glDebugMessageCallback(Debug::glDebugOutput, nullptr);
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

CameraManager* Rendering::GetGameraManager() { return cameraManager; }
ModelManager* Rendering::GetModelManager() { return modelmanager; }

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