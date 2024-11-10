#pragma once

#include <string>
#include <vector>

#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "../objects/Model.hpp"

class ModelManager : UIElement {
public:
	ModelManager();
	~ModelManager();

	void CreateModel(std::string name, std::string modelPath, std::string texturePath, GLenum indexFormat);

	void OnUIRender() override;

private:
	std::vector<Model*>& m_models = Model::GetModelsVector();
};