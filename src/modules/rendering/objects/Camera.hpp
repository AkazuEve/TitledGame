#pragma once

#include "../../debug/Debug.hpp"

#include <string>
#include <vector>

// Glm for math
#include "../../ext/source/glm/glm.hpp"
#include "../../ext/source/glm/gtc/matrix_transform.hpp"
#include "../../ext/source/glm/gtc/type_ptr.hpp"

#include "Shader.hpp"

class Camera {
public:
	// Create camera instance with name
	Camera(std::string name);
	~Camera() = default;
	Camera(const Camera&) = delete;

	// Sends the camera matrix to shader uniform
	void SendDataToShader();

public:
	glm::vec3 position{ 0.0f, 0.0f, -5.0f };
	float fov{ 90.0f };
	float nearPlane{ 0.12f };
	float farPlane{ 50.0f };

	std::string name{};

private:
	glm::mat4 view{ 1.0f };
	glm::mat4 projection{ 1.0f };
};