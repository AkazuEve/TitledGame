#pragma once

#include <vector>

#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "../objects/Shader.hpp"

class ShaderManager : UIElement {
public:
	ShaderManager();
	~ShaderManager();

	Shader* CreateShader(std::string name, std::string vertexFile, std::string fragmentFile);

	static Shader* GetCurrentShader();

	void OnUIRender() override;

private:
	static Shader* m_currentShader;
	std::vector<Shader*> m_shaders{};
};