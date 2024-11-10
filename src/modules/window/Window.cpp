#include "Window.hpp"

void glfwSetWindowCenter(GLFWwindow* window);
void RenderedWindowResizeCallback(GLFWwindow* window, int width, int height);

std::vector<WindowHint> mainWindowHints = {
	{GLFW_DECORATED, GLFW_FALSE},
	{GLFW_CONTEXT_VERSION_MAJOR, 4},
	{GLFW_CONTEXT_VERSION_MINOR, 6},
	{GLFW_RESIZABLE, GLFW_TRUE},
	{GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE},
	#ifdef _DEBUG
		{GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE}
	#endif
};

bool Window::GLFWInitialized = false;
unsigned short Window::WindowInstanceCount = 0;

extern unsigned int renderWidth, renderHeight;

Window::Window(int width, int height, const char* title) {
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

		glfwSetWindowCenter(m_windowPtr);

        glfwSwapInterval(1);

		// Setup resize callback so were always up to date with render resolution
		glfwSetWindowSizeCallback(glfwGetCurrentContext(), RenderedWindowResizeCallback);

		DEBUGPRINT("Window created: " << WindowInstanceCount);
	}
}

Window::~Window() {
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
bool Window::ShouldRun() {
	glfwSwapBuffers(m_windowPtr);
	glfwPollEvents();
	return !glfwWindowShouldClose(m_windowPtr);
}



// Thank you github issues
// https://github.com/glfw/glfw/issues/310
void glfwSetWindowCenter(GLFWwindow* window) {
    // Get window position and size
    int window_x, window_y;
    glfwGetWindowPos(window, &window_x, &window_y);

    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    // Halve the window size and use it to adjust the window position to the center of the window
    window_width *= 0.5f;
    window_height *= 0.5f;

    window_x += window_width;
    window_y += window_height;

    // Get the list of monitors
    int monitors_length;
    GLFWmonitor** monitors = glfwGetMonitors(&monitors_length);

    if (monitors == NULL) {
        // Got no monitors back
        return;
    }

    // Figure out which monitor the window is in
    GLFWmonitor* owner = NULL;
    int owner_x, owner_y, owner_width, owner_height;

    for (int i = 0; i < monitors_length; i++) {
        // Get the monitor position
        int monitor_x, monitor_y;
        glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);

        // Get the monitor size from its video mode
        int monitor_width, monitor_height;
        GLFWvidmode* monitor_vidmode = (GLFWvidmode*)glfwGetVideoMode(monitors[i]);

        if (monitor_vidmode == NULL) {
            // Video mode is required for width and height, so skip this monitor
            continue;

        }
        else {
            monitor_width = monitor_vidmode->width;
            monitor_height = monitor_vidmode->height;
        }

        // Set the owner to this monitor if the center of the window is within its bounding box
        if ((window_x > monitor_x && window_x < (monitor_x + monitor_width)) && (window_y > monitor_y && window_y < (monitor_y + monitor_height))) {
            owner = monitors[i];

            owner_x = monitor_x;
            owner_y = monitor_y;

            owner_width = monitor_width;
            owner_height = monitor_height;
        }
    }

    if (owner != NULL) {
        // Set the window position to the center of the owner monitor
        glfwSetWindowPos(window, owner_x + (owner_width * 0.5f) - window_width, owner_y + (owner_height * 0.5f) - window_height);
    }
}

// Set render resolution to window resolusion when the size changes
void RenderedWindowResizeCallback(GLFWwindow* window, int width, int height) {
    renderWidth = width;
    renderHeight = height;

    DEBUGPRINT("New window size width: " << renderWidth << " height: " << renderHeight);
}