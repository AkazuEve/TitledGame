#pragma once

#include "Debug.hpp"

#include <GLFW/glfw3.h>

#include "../../ext/source/imgui/imgui.h"
#include "../../ext/source/imgui/imgui_impl_glfw.h"
#include "../../ext/source/imgui/imgui_impl_opengl3.h"

#include <vector>
#include <string>
#include <format>

class UIElement {
public:
	UIElement();
	~UIElement();

	virtual void OnUIRender() = 0;

	static std::vector<UIElement*>& GetElementVector() { return elements; };

private:
	static std::vector<UIElement*> elements;
};

namespace UI {
	// Creates ImGui context and sets up for OpenGL3 and GLFW
	void ImGuiInit(const char* version);

	// Frees all the memory and shuts down
	void ImGuiTerminate();

	// Renders the debug GUI
	void RenderUI();
}