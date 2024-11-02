#include <iostream>

#include "../subprojects/NamedLiblary/Rendering.hpp"
#include "modules/Window.hpp"
#include "modules/UI.hpp"

#include <crtdbg.h>

// Vertices coordinates
std::vector<GLfloat> vertices = {
	-0.5f, 0.0f,  0.5f,		0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,		1.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,		0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,		1.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,		0.5f, 1.0f
};

// Indices for vertices order
std::vector<GLushort> indices = {
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

class ModelManager : UIElement {
public:
	ModelManager() {

	}

	~ModelManager() {
		for (Model* model : m_models) {
			delete(model);
		}
	}

	void OnUIRender() override {
		ImGui::Begin("Model Manager");

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

		if (m_models.size() > 0) {
			if (ImGui::TreeNode("Models")) {
				unsigned int i = 0;
				for (Model* model : m_models) {
					if (ImGui::TreeNode(std::string(model->name + "##" + std::to_string(i)).c_str())) {
						i++;
						ImGui::Checkbox("Is Rendered", &model->isRendered);
						ImGui::DragFloat3("Position", (float*)&model->position, 0.05f, -10, 10);
						ImGui::DragFloat3("Rotation", (float*)&model->rotation, 0.5f, -360, 360);
						ImGui::DragFloat3("Scale", (float*)&model->scale, 0.05f, 0, 5);

						if (ImGui::Button("Remove")) {
							DEBUGPRINT("Removed Model from Object manager list: " << model);
							delete(model);
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
private:
	std::vector<Model*>& m_models = Model::GetModelsVector();
};

class CameraManager : UIElement {
public:
	CameraManager() {
		m_currentCamera = new Camera("Default", true);
	}

	~CameraManager() {
		for (Camera* camera : m_cameras) {
			delete(camera);
		}
	}

	void OnUIRender() override {
		ImGui::Begin("Camera Manager");

		static char newCameraName[20]{"Not Default"};

		ImGui::InputText("New Camera Name", newCameraName, 20);

		if (ImGui::Button("Create new camera")) {
			new Camera(newCameraName, false);
		}

		ImGui::Separator();

		for (Camera* camera : m_cameras) {
			if (ImGui::TreeNode(camera->name.c_str())) {
				ImGui::DragFloat3("Position", (float*)&camera->position, 0.05f, -10, 10);
				ImGui::DragFloat("Fov", (float*)&camera->fov, 0.01f, 20.0f, 180.0f);
				ImGui::DragFloat("Near Plane", (float*)&camera->nearPlane, 0.01f, 0.01f, 100.0f);
				ImGui::DragFloat("Far Plane", (float*)&camera->farPlane, 0.01f, 20.0f, 100.0f);
				if (ImGui::Button("Use")) {
					if (camera != m_currentCamera) {
						m_currentCamera->isUsed = false;
						m_currentCamera = camera;
						camera->isUsed = true;
					}
				}

				if (ImGui::Button("Delete")) {
					if (m_currentCamera == camera) {
						for (Camera* cameraSearch : m_cameras) {
							if (cameraSearch != camera) {
								cameraSearch->isUsed = true;
								m_currentCamera = cameraSearch;
							}
						}
					}
					delete(camera);
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

private:
	std::vector<Camera*>& m_cameras = Camera::GetCamerasVector();
	Camera* m_currentCamera = nullptr;
};

// TODO: Move both managers to rendering and comment over everything

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Window mainWindow(1440, 810, "Game Window");
	RenderingInit();
	UI::ImGuiInit("#version 460");

	UI::gBufferColor = GetRenderBufferColorTexture();
	UI::gBufferNormal = GetRenderBufferNormalTexture();
	UI::gBufferDepth = GetRenderBufferDepthTexture();

	ModelManager modelmanager;
	CameraManager cameraManager;

	while (mainWindow.ShouldRun()) {
		Render();
		UI::RenderUI();
	}

	RenderingTerminate();
	UI::ImGuiTerminate();

	return 0;
}