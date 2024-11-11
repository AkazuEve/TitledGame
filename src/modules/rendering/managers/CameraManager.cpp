#include "CameraManager.hpp"

Camera* CameraManager::m_currentCamera = nullptr;
std::vector<Camera*> CameraManager::m_cameras{};

void CameraManager::CreateCamera(std::string name) {
	Camera* camera = new Camera(name);
	m_cameras.push_back(camera);
	m_currentCamera = camera;
}

Camera* CameraManager::GetCurrentCamera() { return m_currentCamera; }

void CameraManager::SetupUI() {
	CreateCamera("Default");
	ImGuiUIManager::AddUIFunction(RenderUI);
}

void CameraManager::FreeMemory() {
	for (Camera* camera : m_cameras) {
		delete(camera);
	}
}

void CameraManager::RenderUI() {
	ImGui::Begin("Camera Manager");

	static char newCameraName[20]{ "Not Default" };

	ImGui::InputText("New Camera Name", newCameraName, 20);

	if (ImGui::Button("Create new camera")) {
		CreateCamera(newCameraName);
	}

	ImGui::Separator();

	for (Camera* camera : m_cameras) {
		if (ImGui::TreeNode(camera->name.c_str())) {
			ImGui::DragFloat3("Position", (float*)&camera->position, 0.05f, -10, 10);
			ImGui::DragFloat("Fov", (float*)&camera->fov, 0.01f, 20.0f, 180.0f);
			ImGui::DragFloat("Near Plane", (float*)&camera->nearPlane, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat("Far Plane", (float*)&camera->farPlane, 0.01f, 20.0f, 100.0f);

			ImGui::Checkbox("Looking around", &camera->firstPress);

			ImGui::SliderFloat("Speed", &camera->speed, 1.0f, 50.0f);
			ImGui::SliderFloat("Sensitivity", &camera->sensitivity, 10.0f, 500.0f);
			ImGui::DragFloat3("Orientation", (float*)&camera->orientation, 0.05f, -10, 10);

			if (ImGui::Button("Use")) {
				m_currentCamera = camera;
			}

			ImGui::SameLine();

			if (m_cameras.size() > 1) {
				if (ImGui::Button("Delete")) {
					// Remove this pointer from camera pointer vector
					std::vector<Camera*>::iterator position = std::find(m_cameras.begin(), m_cameras.end(), camera);
					if (position != m_cameras.end()) {
						m_cameras.erase(position);
						DEBUGPRINT("Removed Camera from model poiter vector: " << camera);

						m_currentCamera = m_cameras[0];
						delete(camera);
					}
				}
				ImGui::SameLine();
			}
			if (ImGui::Button("Reset Transform")) {
				camera->position = glm::vec3(0.0f, 0.0f, -5.0f);
				camera->orientation = glm::vec3(0.0f, 0.0f, 1.0f);
			}

			ImGui::TreePop();
		}
	}
	ImGui::End();
}