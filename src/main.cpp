#include <iostream>

#include "../subprojects/NamedLiblary/Rendering.hpp"
#include "modules/Window.hpp"
#include "modules/UI.hpp"

// Vertices coordinates
std::vector<GLfloat> vertices = {
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, // Lower left corner
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f, // Upper left corner
	 0.5f,  0.5f, 0.0f,		1.0f, 1.0f, // Upper right corner
	 0.5f, -0.5f, 0.0f,		1.0f, 0.0f  // Lower right corner
};

// Indices for vertices order
std::vector<GLushort> indices = {
	0, 2, 1, // Upper triangle
	0, 3, 2 // Lower triangle
};

class ModelManager : UIElement {
public:
	ModelManager() {

	}
	~ModelManager() {
		for (Model* Model : m_models) {
			Model->~Model();
		}
	}

	void OnUIRender() {
		static char name[20]{"Cat"};
		static char textureName[20]{"pop_cat"};
		ImGui::InputText("Name", name, 20);
		ImGui::InputText("Texture Name", textureName, 20);
		if (ImGui::Button("Create Object")) {
			static std::string pre = "res/textures/";
			static std::string post = ".png";

			std::string texturepPath = pre + textureName + post;

			Model* tmpPtr = new Model;

			tmpPtr->AddMesh(name, vertices, indices, GL_UNSIGNED_SHORT);
			tmpPtr->AddTexture(GL_TEXTURE0, texturepPath);
		}

		if (ImGui::TreeNode("Models")) {
			unsigned int i = 0;
			for (Model* model : m_models) {
				if (ImGui::TreeNode(std::string(model->name + "##" + std::to_string(i)).c_str())) {
					i++;
					ImGui::Checkbox("Is Rendered", &model->isRendered);
					ImGui::SliderFloat3("Position", (float*)&model->position, -100, 100);
					ImGui::SliderFloat3("Rotation", (float*)&model->rotation, -100, 100);
					ImGui::SliderFloat3("Scale", (float*)&model->scale, -100, 100);

					if (ImGui::Button("Remove")) {
						std::vector<Model*>::iterator position = std::find(m_models.begin(), m_models.end(), model);
						if (position != m_models.end()) {
							model->~Model();
							DEBUGPRINT("Removed Model from Object manager list: " << model);
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
private:
	std::vector<Model*>& m_models = Model::GetModelsVector();
};

int main() {
	Window mainWindow(1440, 810, "Game Window");
	RenderingInit();
	UI::ImGuiInit("#version 460");

	UI::gBufferColor = GetRenderBufferColorTexture();
	UI::gBufferNormal = GetRenderBufferNormalTexture();
	UI::gBufferDepth = GetRenderBufferDepthTexture();

	ModelManager manager;

	//Object cat("Popcat", "res/textures/pop_cat.png");
	//Object brick("Brick", "res/textures/brick.png");
	//Object planks("Planks", "res/textures/planks.png");

	while (mainWindow.ShouldRun()) {
		Render();
		UI::RenderUI();
	}

	RenderingTerminate();
	UI::ImGuiTerminate();

	return 0;
}