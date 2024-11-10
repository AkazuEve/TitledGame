#include "Camera.hpp"

extern unsigned int renderWidth, renderHeight;
extern Shader* currentShader;

std::vector<Camera*> Camera::m_camers;

Camera::Camera(std::string name, bool enabled) {
	this->name = name;
	this->isUsed = enabled;

	DEBUGPRINT("Added " << name << " Camera to camera pointer list: " << this);
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
	glUniformMatrix4fv(currentShader->GetCameraUniformLocation(), 1, GL_FALSE, glm::value_ptr(result));
}