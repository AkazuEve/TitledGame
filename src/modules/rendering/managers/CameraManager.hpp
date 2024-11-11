#pragma once

#include <vector>
#include <string>

// UI class to render mroe advanced ui than im capable of
#include "../../src/modules/rendering/UI/ImGuiUI.hpp"

#include "../objects/Camera.hpp"

class CameraManager {
public:
	CameraManager() = delete;
	CameraManager(const CameraManager&) = delete;
	~CameraManager() = delete;

	static void CreateCamera(std::string name);

	static Camera* GetCurrentCamera();

	static void SetupUI();
	static void FreeMemory();

	static void RenderUI();
private:
	static Camera* m_currentCamera;
	static std::vector<Camera*> m_cameras;
};