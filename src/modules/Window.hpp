#pragma once

// Standard lib
#include <vector>

#include "Debug.hpp"

#include <GLFW/glfw3.h>

// I wanted it so I did it
typedef GLFWwindow* GLFWwindowPtr;

struct WindowHint {
	int hint;
	int value;
};

class Window {
public:
    Window(int width, int height, const char* title);

	~Window();

	// Returns true if window sould keep running and handles buffer swapping and elents
    bool ShouldRun();

private:
	GLFWwindowPtr m_windowPtr = nullptr;

	static bool GLFWInitialized;
	static unsigned short WindowInstanceCount;
};