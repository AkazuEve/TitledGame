#pragma once

#include <vector>

#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "../objects/Shader.hpp"

class ShaderManager : UIElement {
public:
	ShaderManager();
	~ShaderManager();

	void OnUIRender() override;

private:
	std::vector<Shader*>& m_shaders = Shader::GetShadersVector();
};