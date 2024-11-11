#pragma once

#include "../../debug/Debug.hpp"

#include <GLFW/glfw3.h>

#include "../../ext/source/imgui/imgui.h"
#include "../../ext/source/imgui/imgui_impl_glfw.h"
#include "../../ext/source/imgui/imgui_impl_opengl3.h"

#include <vector>
#include <string>
#include <format>

class ImGuiUIManager {
public:
	ImGuiUIManager() = delete;
	~ImGuiUIManager() = delete;

	static void InitUI(const char* version);
	static void FreeMemory();

	static void AddUIFunction(void(*func)());

	static void Render();

	static std::vector<void(*)()> uiFunctions;
private:

};