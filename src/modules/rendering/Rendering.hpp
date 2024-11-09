#pragma once

// Standard lib
#include <vector>


#define DEBUGPRINT(x) std::cout << x << std::endl;

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"


// GLFW for window resize callback
#include "../../ext/precompiled/include/GLFW/glfw3.h"

// UI class to render mroe advanced ui than im capable of
#include "../../src/modules/UI.hpp"

#include "Structs.hpp"

#include "Texture.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Mesh.hpp"

class ShaderManager : UIElement {
public:
	ShaderManager();
	~ShaderManager();

	void OnUIRender() override;

private:
	std::vector<Shader*>& m_shaders = Shader::GetShadersVector();
};

class ModelManager : UIElement {
public:
	ModelManager();
	~ModelManager();

	void CreateModel(std::string name, std::string modelPath, std::string texturePath, GLenum indexFormat);

	void OnUIRender() override;

private:
	std::vector<Model*>& m_models = Model::GetModelsVector();
};

class CameraManager : UIElement {
public:
	CameraManager();

	~CameraManager();

	void CreateCamera(std::string name, bool enabled);

	void OnUIRender() override;
private:
	std::vector<Camera*>& m_cameras = Camera::GetCameraVector();
};

namespace Rendering {
	// Initializes all rendering stuff including a basic shader and render buffer
	void Init();

	// Frees all memory and deletes all OpenGL objects
	void Terminate();

	void CreateCamera(std::string name, bool enabled);
	void CreateModel(std::string name, std::string modelPath, std::string texturePath, GLenum indexFormat);

	//Render all data stored in mesh classes.
	//Requires an existing opengl rendering context.
	void Render();
}
