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

void RendererUI();

#pragma region Variables

// Default clear color
glm::vec3 defaultClearColor = { 0.2f, 0.2f, 0.6f };

// Render buffer stuff
GLuint framebuffer{ 0 }, framebufferColorTexture{ 0 }, framebufferDepthTexture{ 0 };

unsigned int renderWidth{ 1440 }, renderHeight{ 810 };
unsigned int viewportRenderWidth{ 100 }, viewportRenderHeight{ 100 };

#pragma endregion

void Rendering::Init() {
	// Load all OpenGL functions
	gladLoadGL();

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &framebufferColorTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWidth, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferColorTexture, 0);

	glGenTextures(1, &framebufferDepthTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, renderWidth, renderHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebufferDepthTexture, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	ImGuiUIManager::InitUI("#version 460");
	ImGuiUIManager::AddUIFunction(RendererUI);

	ShaderManager::SetupUI();
	ModelManager::SetupUI();
	CameraManager::SetupUI();

	// Turns out depth buffer wont work when its not enabled :>
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, 1.0f);

	ShaderManager::CreateShader("Position Debug", "res/shaders/default.vert", "res/shaders/positionDebug.frag");
	ShaderManager::CreateShader("Normal Debug", "res/shaders/default.vert", "res/shaders/normalDebug.frag");
	ShaderManager::CreateShader("Default", "res/shaders/default.vert", "res/shaders/default.frag");

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
	ImGuiUIManager::FreeMemory();

	ShaderManager::FreeMemory();
	ModelManager::FreeMemory();
	CameraManager::FreeMemory();
}

void Rendering::Render() {
	// Bind the render buffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindTexture(GL_TEXTURE_2D, framebufferColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewportRenderWidth, viewportRenderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, framebufferDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, viewportRenderWidth, viewportRenderHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);


	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set rendering resolution
	glViewport(0, 0, viewportRenderWidth, viewportRenderHeight);

	ShaderManager::GetCurrentShader()->BindShader();

	CameraManager::GetCurrentCamera()->Inputs();
	CameraManager::GetCurrentCamera()->SendDataToShader();

	// Render data form all instances of Model if enabled
	for (Model* model : ModelManager::GetModelPointerVector()) {
		if (model->isRendered) {
			model->BindModel();
			glDrawElements(GL_TRIANGLES, (GLsizei)model->GetIndexBufferSize(), GL_UNSIGNED_INT, 0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, renderWidth, renderHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGuiUIManager::Render();
}

void RendererUI() {
	ImGui::Begin("Renderer");
	static ImVec2 viewportSize;


	if (ImGui::TreeNode("Culling Options")) {
		if (ImGui::Button("Enable Culling")) {
			glEnable(GL_CULL_FACE);
		}
		if (ImGui::Button("Disable Culling")) {
			glDisable(GL_CULL_FACE);
		}

		if (ImGui::Button("Front culling")) {
			glCullFace(GL_FRONT);
		}
		if (ImGui::Button("Back culling")) {
			glCullFace(GL_BACK);
		}

		if (ImGui::Button("Front Face CCW")) {
			glFrontFace(GL_CCW);
		}
		if (ImGui::Button("Front Face CW")) {
			glFrontFace(GL_CW);
		}
		ImGui::TreePop();
	}

	ImGui::End();
	ImGui::Begin("Color Buffer");

	viewportSize = ImGui::GetContentRegionAvail();
	viewportRenderWidth = viewportSize.x;
	viewportRenderHeight = viewportSize.y;

	ImGui::Image((ImTextureID)framebufferColorTexture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();

	ImGui::Begin("Depth Buffer");

	viewportSize = ImGui::GetContentRegionAvail();
	viewportRenderWidth = viewportSize.x;
	viewportRenderHeight = viewportSize.y;

	ImGui::Image((ImTextureID)framebufferDepthTexture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
}