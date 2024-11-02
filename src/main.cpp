#include <iostream>

#include "../subprojects/NamedLiblary/Rendering.hpp"
#include "modules/Window.hpp"
#include "modules/UI.hpp"

// Vertices coordinates
std::vector<GLfloat> vertices = {
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, // Lower left corner
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f, // Upper left corner
	 0.5f,  0.5f, 0.0f,		1.0f, 1.0f, // Upper right corner
	 0.5f, -0.5f, 0.0f,		1.0f, 0.0f  // Lower right corner
};

// Indices for vertices order
std::vector<GLushort> indices = {
	0, 2, 1, // Upper triangle
	0, 3, 2 // Lower triangle
};


class Thing: UIElement {
public:
	Thing(std::string name, std::string texture) {
		m_name = name.c_str();
		model.AddMesh(vertices, indices, GL_UNSIGNED_SHORT);
		model.AddTexture(GL_TEXTURE0, texture);
		model.isRendered = true;
	}

	void OnUIRender() {
		if (ImGui::TreeNode(m_name.c_str())) {
			ImGui::Checkbox("Is Rendered", &model.isRendered);
			ImGui::SliderFloat3("Position", (float*)&model.position, -100, 100);
			ImGui::SliderFloat3("Rotation", (float*)&model.rotation, -100, 100);
			ImGui::SliderFloat3("Scale", (float*)&model.scale, -100, 100);

			ImGui::TreePop();
		}
	}

private:
	std::string m_name;
	Model model;
};


// When you come back comment everything dumbass


int main() {
	Window mainWindow(1440, 810, "Game Window");
	RenderingInit();
	UI::ImGuiInit("#version 460");

	UI::gBufferColor = GetRenderBufferColorTexture();
	UI::gBufferNormal = GetRenderBufferNormalTexture();
	UI::gBufferDepth = GetRenderBufferDepthTexture();

	Thing cat("Popcat", "res/textures/pop_cat.png");
	Thing brick("Brick", "res/textures/brick.png");
	Thing planks("Planks", "res/textures/planks.png");

	while (mainWindow.ShouldRun()) {
		Render();
		UI::RenderUI();
	}

	RenderingTerminate();
	UI::ImGuiTerminate();

	return 0;
}