#pragma once

#include "../../debug/Debug.hpp"

#include <string>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
// Glm for math
#include "../../ext/source/glm/glm.hpp"
#include "../../ext/source/glm/gtc/matrix_transform.hpp"
#include "../../ext/source/glm/gtc/type_ptr.hpp"
#include "../../ext/source/glm/gtx/rotate_vector.hpp"
#include "../../ext/source/glm/gtx/vector_angle.hpp"

#include "../../ext/source/imgui/imgui.h"

#include "Shader.hpp"

class Camera {
public:
	// Create camera instance with name
	Camera(std::string name);
	~Camera() = default;
	Camera(const Camera&) = delete;

	// Sends the camera matrix to shader uniform
	void SendDataToShader();

	void Inputs();

public:
	glm::vec3 position{ 0.0f, 0.0f, -5.0f };
	float fov{ 90.0f };
	float nearPlane{ 0.12f };
	float farPlane{ 50.0f };

	float speed{ 10.0f };
	float sensitivity{ 100.0f };
	glm::vec3 orientation{ 0.0f, 0.0f, 1.0f };

	bool firstPress{ false };
	bool lookingAround{ false };

	std::string name{};

private:
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };

	glm::mat4 view{ 1.0f };
	glm::mat4 projection{ 1.0f };
};