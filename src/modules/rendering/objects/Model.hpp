#pragma once

#include <string>
#include <vector>

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"

// Glm for math
#include "../../ext/source/glm/glm.hpp"
#include "../../ext/source/glm/gtc/matrix_transform.hpp"
#include "../../ext/source/glm/gtc/type_ptr.hpp"

#include "Texture.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

// Model class contains mesh data and textures. After creation data gets rendered when calling Render()
class Model {
public:
	Model() = default;
	~Model();
	Model(const Model&) = delete;

	void AddMesh(std::string name, const MeshData& data);

	void AddTexture(std::string textuprePath, GLenum textureSlot);

	void BindModel();

	size_t GetIndexBufferSize() { return m_mesh.GetIndexBufferSize(); }

public:
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };

	bool isRendered = true;

	std::string name{};

private:
	Mesh m_mesh;
	std::vector<Texture*> m_textures;

	glm::mat4 m_modelMatrix{ 1.0 };
	glm::mat4 m_modelNormalMatrix{ 1.0 };

	static glm::mat4 translationMatrix;
	static glm::mat4 rotationMatrix;
	static glm::mat4 scaleMatrix;

	static glm::mat4 sm_identityMatrix;
};