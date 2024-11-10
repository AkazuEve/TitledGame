#include "Rendering.hpp"

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
ShaderManager* shaderManager = nullptr;
ModelManager* modelmanager = nullptr;
CameraManager* cameraManager = nullptr;

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

	shaderManager = new ShaderManager;
	modelmanager = new ModelManager;
	cameraManager = new CameraManager;

	shaderManager->CreateShader("Position Debug", "res/shaders/default.vert", "res/shaders/positionDebug.frag");
	shaderManager->CreateShader("Normal Debug", "res/shaders/default.vert", "res/shaders/normalDebug.frag");
	shaderManager->CreateShader("Default", "res/shaders/default.vert", "res/shaders/default.frag");

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

	delete(shaderManager);
	delete(modelmanager);
	delete(cameraManager);
}

ModelManager* Rendering::GetModelManager() { return modelmanager; }
CameraManager* Rendering::GetCameraManager() { return cameraManager; }

void Rendering::Render() {
	// Bind the render buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set rendering resolution
	glViewport(0, 0, renderWidth, renderHeight);

	ShaderManager::GetCurrentShader()->BindShader();
	CameraManager::GetCurrentCamera()->SendDataToShader();

	// Render data form all instances of Model if enabled
	for (Model* model : ModelManager::GetModelPointerVector()) {
		if (model->isRendered) {
			model->BindModel();
			glDrawElements(GL_TRIANGLES, (GLsizei)model->GetIndexBufferSize(), GL_UNSIGNED_INT, 0);
		}
	}

	UI::RenderUI();
}