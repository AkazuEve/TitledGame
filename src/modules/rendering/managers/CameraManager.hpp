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

	void CreateCamera(std::string name);

	static Camera* GetCurrentCamera();

	void OnUIRender() override;
private:
	static Camera* m_currentCamera;
	std::vector<Camera*> m_cameras{};
};