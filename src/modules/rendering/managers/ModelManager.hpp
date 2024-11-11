#pragma once

#include <string>
#include <vector>

#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "../objects/Model.hpp"

#include "../../debug/Console.hpp"

class ModelManager {
public:
	ModelManager() = delete;
	ModelManager(const ModelManager&) = delete;
	~ModelManager() = delete;

	static void SetupUI();
	static void FreeMemory();

	static Model* CreateModel(std::string name, std::string modelPath, std::string texturePath);
	static Model* CreateModel(std::string modelPath, std::string texturePath);

	static std::vector<Model*>& GetModelPointerVector();

	static void RenderUI();

private:
	static std::vector<Model*> m_models;
};