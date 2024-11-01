#pragma once

#include "../../ext/source/imgui/imgui.h"
#include "../../ext/source/imgui/imgui_impl_glfw.h"
#include "../../ext/source/imgui/imgui_impl_opengl3.h"

#include <vector>
#include <string>
#include <format>

class UIElement {
public:
	UIElement() {
		elements.push_back(this);
	}
	virtual void OnUIRender() = 0;

	static std::vector<UIElement*> elements;

};

std::vector <UIElement*> UIElement::elements{};

namespace UI {
	unsigned int gBufferColor{ 0 };
	unsigned int gBufferNormal{ 0 };
	unsigned int gBufferDepth{ 0 };

	unsigned int currentBuffer{ 0 };

	// Creates ImGui context and sets up for OpenGL3 and GLFW
	void ImGuiInit(const char* version) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
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

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Debug buffer view"))
			{
				if (ImGui::Button("Color Buffer")) {
					currentBuffer = gBufferColor;
				}
				if (ImGui::Button("Normal Buffer")) {
					currentBuffer = gBufferNormal;
				}
				if (ImGui::Button("Depth Buffer")) {
					currentBuffer = gBufferDepth;
				}
				if (ImGui::Button("None")) {
					currentBuffer = 0;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar ();
		}

		if (currentBuffer) {
			ImGui::Begin("Debug buffer view", nullptr, ImGuiWindowFlags_MenuBar);
			ImGui::Image(reinterpret_cast<ImTextureID>(currentBuffer), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
		}


		if (UIElement::elements.size() > 1) {
			ImGui::Begin("Objects");

			for (UIElement* element : UIElement::elements) {
				element->OnUIRender();
			}

			ImGui::End();
		}
	
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}