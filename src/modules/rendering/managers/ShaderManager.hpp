#pragma once

#include <vector>

#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "../objects/Shader.hpp"

class ShaderManager {
public:
	ShaderManager() = delete;
	ShaderManager(const ShaderManager&) = delete;
	~ShaderManager() = delete;

	static void SetupUI();
	static void FreeMemory();

	static Shader* CreateShader(std::string name, std::string vertexFile, std::string fragmentFile);

	static Shader* GetCurrentShader();

	static void RenderUI();

private:
	static Shader* m_currentShader;
	static std::vector<Shader*> m_shaders;
};