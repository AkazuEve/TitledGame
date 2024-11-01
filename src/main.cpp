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

class Object: UIElement {
public:
	Object(const char* name) {
		tris = new Mesh(vertices, indices, &rendered);
		this->name = name;
	}

	~Object() {
		delete(tris);
	}

	virtual void OnUIRender() {
		ImGui::Text(name);
		ImGui::Checkbox(name, &rendered);
	}

private:
	bool rendered = true;
	Mesh* tris;

	const char* name;
};


int main() {
	Window mainWindow(1440, 810, "Game Window");
	RenderingInit();
	UI::ImGuiInit("#version 460");

	UI::gBufferColor = GetRenderBufferColorTexture();
	UI::gBufferNormal = GetRenderBufferNormalTexture();
	UI::gBufferDepth = GetRenderBufferDepthTexture();

	Object yeet("One");

	while (mainWindow.ShouldRun()) {
		Render();
		UI::RenderUI();
	}

	RenderingTerminate();
	UI::ImGuiTerminate();

	return 0;
}