#include "ModelManager.hpp"

#include "../../resources/ResourceManager.hpp"

std::vector<Model*> ModelManager::m_models{};

ModelManager::ModelManager() {
}

ModelManager::~ModelManager() {
	for (Model* model : m_models) {
		delete(model);
	}
}

Model* ModelManager::CreateModel(std::string name, std::string modelPath, std::string texturePath) {
	Model* model = new Model;

	MeshData data = ResourceManager::LoadPly(modelPath);

	model->AddMesh(name, data);
	model->AddTexture(GL_TEXTURE0, texturePath);

	m_models.push_back(model);

	return model;
}

Model* ModelManager::CreateModel(std::string modelPath, std::string texturePath) {
	Model* model = new Model;

	MeshData data = ResourceManager::LoadPly(modelPath);

	char* name = (char*)malloc(5);

	if (name) {
		name[0] = rand() % 65 + 25;
		name[1] = rand() % 65 + 25;
		name[2] = rand() % 65 + 25;
		name[3] = rand() % 65 + 25;
		name[4] = 0;

		model->AddMesh(name, data);

		free(name);
	}

	model->AddTexture(GL_TEXTURE0, texturePath);

	m_models.push_back(model);

	return model;
}

std::vector<Model*>& ModelManager::GetModelPointerVector() { return m_models; }

void ModelManager::OnUIRender() {
	ImGui::Begin("Model Manager");

	static char name[20]{ "Test" };
	static char textureName[20]{ "brick" };
	static char modelName[20]{ "untitled" };
	ImGui::InputText("Name", name, 20);
	ImGui::InputText("Texture Name", textureName, 20);
	ImGui::InputText("Model Name", modelName, 20);
	if (ImGui::Button("Create Object")) {
		std::string texturePath = std::string("res/textures/") + textureName + std::string(".png");
		std::string modelPath = std::string("res/models/") + modelName + std::string(".ply");

		CreateModel(name, modelPath, texturePath);
	}

	ImGui::Separator();

	if (m_models.size() > 0) {
		for (Model* model : m_models) {
			if (ImGui::TreeNode(model->name.c_str())) {
				ImGui::Checkbox("Is Rendered", &model->isRendered);
				ImGui::DragFloat3("Position", (float*)&model->position, 0.05f, -10, 10);
				ImGui::DragFloat3("Rotation", (float*)&model->rotation, 0.5f, -360, 360);
				ImGui::DragFloat3("Scale", (float*)&model->scale, 0.05f, 0, 5);

				if (ImGui::Button("Remove")) {
					DEBUGPRINT("Removed Model from Object manager list: " << model);


					// Remove this pointer from models vector
					std::vector<Model*>::iterator position = std::find(m_models.begin(), m_models.end(), model);
					if (position != m_models.end()) {
						m_models.erase(position);
						delete(model);
						DEBUGPRINT("Removed Model from model list: " << model);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset Transform")) {
					model->position = glm::vec3(0.0f);
					model->rotation = glm::vec3(0.0f);
					model->scale = glm::vec3(1.0f);
				}

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}