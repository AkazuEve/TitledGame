#include "ImGuiUI.hpp"

std::vector <UIElement*> UIElement::elements;

UIElement::UIElement() {
		elements.push_back(this);
		DEBUGPRINT("Created UIElement: " << this);
	}

UIElement::~UIElement() {
	std::vector<UIElement*>::iterator position = std::find(elements.begin(), elements.end(), this);
	if (position != elements.end()) {
		elements.erase(position);
		DEBUGPRINT("Removed UIElement from list: " << this);
	}
}

namespace UI {
	// Creates ImGui context and sets up for OpenGL3 and GLFW
	void ImGuiInit(const char* version) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

		ImGui_ImplOpenGL3_Init(version);
		ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);

	}

	// Frees all the memory and shuts down
	void ImGuiTerminate() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	// Renders the debug GUI
	void RenderUI() {
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

		for (UIElement* element : UIElement::GetElementVector()) {
			element->OnUIRender();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}