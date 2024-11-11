#pragma once

// Standard lib
#include <vector>

#include "../debug/Debug.hpp"

#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char* title);
	~Window();

	// Returns true if window sould keep running and handles buffer swapping and elents
    bool ShouldRun();

private:
	typedef GLFWwindow* GLFWwindowPtr;

	GLFWwindowPtr m_windowPtr = nullptr;

	static bool GLFWInitialized;
	static unsigned short WindowInstanceCount;
};