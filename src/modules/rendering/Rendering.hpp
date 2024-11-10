#pragma once

// Standard lib
#include <vector>

#include "../debug/Debug.hpp"

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"

// GLFW for window resize callback
#include "../../ext/precompiled/include/GLFW/glfw3.h"

// UI class to render more advanced ui than im capable of
#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "managers/ShaderManager.hpp"
#include "managers/ModelManager.hpp"
#include "managers/CameraManager.hpp"

namespace Rendering {
	// Initializes all rendering stuff including a basic shader and render buffer
	void Init();

	// Frees all memory and deletes all OpenGL objects
	void Terminate();

	ModelManager* GetModelManager();
	CameraManager* GetCameraManager();

	//Render all data stored in mesh classes.
	//Requires an existing opengl rendering context.
	void Render();
}
