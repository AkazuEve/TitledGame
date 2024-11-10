#pragma once

#include <string>
#include <vector>

#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "../objects/Model.hpp"

class ModelManager : UIElement {
public:
	ModelManager();
	~ModelManager();

	Model* CreateModel(std::string name, std::string modelPath, std::string texturePath);
	Model* CreateModel(std::string modelPath, std::string texturePath);

	static std::vector<Model*>& GetModelPointerVector();

	void OnUIRender() override;

private:
	static std::vector<Model*> m_models;
};