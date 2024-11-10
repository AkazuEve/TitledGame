#include "Camera.hpp"

#include "../managers/ShaderManager.hpp"

extern unsigned int renderWidth, renderHeight;

Camera::Camera(std::string name): name(name) {
	DEBUGPRINT("Created camera: " << this);
}

void Camera::SendDataToShader() {
	// Reset 
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	// Calculate view and projection matrix
	view = glm::translate(view, position);
	projection = glm::perspective(glm::radians(fov), (float)(renderWidth / renderHeight), nearPlane, farPlane);

	static glm::mat4 result{ 0.0f };
	result = projection * view;
	glUniformMatrix4fv(ShaderManager::GetCurrentShader()->GetCameraUniformLocation(), 1, GL_FALSE, glm::value_ptr(result));
}