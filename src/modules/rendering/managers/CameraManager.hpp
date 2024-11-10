#pragma once

#include <vector>
#include <string>

// UI class to render mroe advanced ui than im capable of
#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "../objects/Camera.hpp"

class CameraManager : UIElement {
public:
	CameraManager();
	~CameraManager();

	void CreateCamera(std::string name, bool enabled);

	void SetCurrentCameraPosition(glm::vec3 position);

	void OnUIRender() override;
private:
	std::vector<Camera*>& m_cameras = Camera::GetCameraVector();
};