#pragma once

// Standard lib
#include <vector>

// GLFW for cross platform window creation
#include <GLFW/glfw3.h>

#include "Debug.hpp"

// I wanted it so I did it
typedef GLFWwindow* GLFWwindowPtr;

struct WindowHint {
	int hint;
	int value;
};

std::vector<WindowHint> mainWindowHints = {
	{GLFW_CONTEXT_VERSION_MAJOR, 4},
	{GLFW_CONTEXT_VERSION_MINOR, 6},
	{GLFW_RESIZABLE, GLFW_TRUE},
	{GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE},
	{GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE}
};

class Window {
public:
	Window(int width, int height,const char* title) noexcept {
		// Check if GLFW did not fail to initialize
		if (!GLFWInitialized) glfwInit();
		DEBUGPRINT("GLFW initialized");

		//Set all window hints
		for (WindowHint hint : mainWindowHints) {
			glfwWindowHint(hint.hint, hint.value);
		}

		// Create window and check if it worked and increment instance count
		m_windowPtr = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (m_windowPtr) {
			WindowInstanceCount++;
			glfwMakeContextCurrent(m_windowPtr);
			DEBUGPRINT("Window created: " << WindowInstanceCount);
		}
	}

	~Window() {
		// Destroy window and decrement instance count
		glfwDestroyWindow(m_windowPtr);
		DEBUGPRINT("Window Destroyed: " << WindowInstanceCount);
		WindowInstanceCount--;

		// If no widows exist terminate GLFW
		if (WindowInstanceCount == 0) {
			glfwTerminate();
			DEBUGPRINT("GLFW Terminated");
		}
	}

	// Returns true if window sould keep running and handles buffer swapping and elents
	bool ShouldRun() {
		glfwSwapBuffers(m_windowPtr);
		glfwPollEvents();
		return !glfwWindowShouldClose(m_windowPtr);
	}

private:
	GLFWwindowPtr m_windowPtr = nullptr;

	static bool GLFWInitialized;
	static unsigned short WindowInstanceCount;
};

bool Window::GLFWInitialized = false;
unsigned short Window::WindowInstanceCount = 0;