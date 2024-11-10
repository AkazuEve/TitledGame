#include "CameraManager.hpp"

extern Camera* currentCamera;

// Creates a defaualt camera and sets it as active
CameraManager::CameraManager() {
	m_cameras.push_back(new Camera("Default", true));
	currentCamera = m_cameras[0];
}

// Deallocates all memory from camera vectors
CameraManager::~CameraManager() {
	for (Camera* camera : m_cameras) {
		delete(camera);
	}
}

void CameraManager::CreateCamera(std::string name, bool enabled) {
	m_cameras.push_back(new Camera(name, enabled));
}

void CameraManager::SetCurrentCameraPosition(glm::vec3 position) {
	currentCamera->position = position;
}

void CameraManager::OnUIRender() {
	ImGui::Begin("Camera Manager");

	static char newCameraName[20]{ "Not Default" };

	ImGui::InputText("New Camera Name", newCameraName, 20);

	if (ImGui::Button("Create new camera")) {
		m_cameras.push_back(new Camera(newCameraName, true));
	}

	ImGui::Separator();

	for (Camera* camera : m_cameras) {

		if (ImGui::TreeNode(camera->name.c_str())) {
			ImGui::DragFloat3("Position", (float*)&camera->position, 0.05f, -10, 10);
			ImGui::DragFloat("Fov", (float*)&camera->fov, 0.01f, 20.0f, 180.0f);
			ImGui::DragFloat("Near Plane", (float*)&camera->nearPlane, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat("Far Plane", (float*)&camera->farPlane, 0.01f, 20.0f, 100.0f);

			if (ImGui::Button("Use")) {
				currentCamera = camera;
			}

			ImGui::SameLine();

			if (m_cameras.size() > 1) {
				if (ImGui::Button("Delete")) {
					// Remove this pointer from camera pointer vector
					std::vector<Camera*>::iterator position = std::find(m_cameras.begin(), m_cameras.end(), camera);
					if (position != m_cameras.end()) {
						m_cameras.erase(position);
						DEBUGPRINT("Removed Camera from model poiter vector: " << this);

						currentCamera = m_cameras[0];
						delete(camera);
					}
				}
				ImGui::SameLine();
			}
			if (ImGui::Button("Reset Transform")) {
				camera->position = glm::vec3(0.0f, 0.0f, -5.0f);
			}

			ImGui::TreePop();
		}
	}
	ImGui::End();
}