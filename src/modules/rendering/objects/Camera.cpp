#include "Camera.hpp"

#include "../managers/ShaderManager.hpp"

extern unsigned int viewportRenderWidth, viewportRenderHeight;

Camera::Camera(std::string name): name(name) {
	DEBUGPRINT("Created camera: " << this);
}

void Camera::SendDataToShader() {
	// Reset 
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	// Calculate view and projection matrix
	view = glm::lookAt(position, position + orientation, up);

	if(viewportRenderWidth && viewportRenderHeight)
		projection = glm::perspective(glm::radians(fov), (float)(viewportRenderWidth / viewportRenderHeight), nearPlane, farPlane);
	else
		projection = glm::perspective(glm::radians(fov), 1.0f, nearPlane, farPlane);


	static glm::mat4 result{ 0.0f };
	result = projection * view;
	glUniformMatrix4fv(ShaderManager::GetCurrentShader()->GetCameraUniformLocation(), 1, GL_FALSE, glm::value_ptr(result));
}

void Camera::Inputs()
{
	GLFWwindow* window = glfwGetCurrentContext();

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += speed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position += speed * -glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position += speed * -orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += speed * glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position += speed * glm::vec3(0.0f, -1.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		lookingAround = true;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		lookingAround = false;

	// Handles mouse inputs
	if (lookingAround)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Prevents camera from jumping on the first click
		if (firstPress)
		{
			glfwSetCursorPos(window, (viewportRenderWidth / 2), (viewportRenderHeight / 2));
			firstPress = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (viewportRenderHeight / 2)) / viewportRenderHeight;
		float rotY = sensitivity * (float)(mouseX - (viewportRenderWidth / 2)) / viewportRenderWidth;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (viewportRenderWidth / 2), (viewportRenderHeight / 2));
	}
	else if (!lookingAround)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstPress = true;
	}
}