#include "ImGuiUI.hpp"

std::vector<void(*)()> ImGuiUIManager::uiFunctions{};

void ImGuiUIManager::AddUIFunction(void(*func)()) { uiFunctions.push_back(func); }

void ImGuiUIManager::InitUI(const char* version) {
	// Creates ImGui context and sets up for OpenGL3 and GLFW
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	ImGui_ImplOpenGL3_Init(version);
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
}

void ImGuiUIManager::FreeMemory() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiUIManager::Render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::Button("Exit")) {
					glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	ImGui::DockSpaceOverViewport();

	for (void(*func)() : ImGuiUIManager::uiFunctions) {
		func();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}