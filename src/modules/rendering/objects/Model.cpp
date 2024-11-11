#include "Model.hpp"

#include "../managers/ShaderManager.hpp"

glm::mat4 Model::sm_identityMatrix{ 1.0f };

glm::mat4 Model::translationMatrix{ 1.0f };
glm::mat4 Model::rotationMatrix{ 1.0f };
glm::mat4 Model::scaleMatrix{ 1.0f };

Model::~Model() {
	// Destroy all allocated textures
	for (Texture* texture : m_textures) {
		delete(texture);
	}
}

void Model::AddMesh(std::string name, const MeshData& data) {
	this->name = name;
	m_mesh.LoadMeshData(data);
}

void Model::AddTexture(std::string textuprePath, GLenum textureSlot) {
	// Create new texture
	Texture* texture = new Texture();

	// Setup data and push to texture vector
	texture->LoadTextureData(textuprePath, textureSlot);
	m_textures.push_back(texture);
}

void Model::BindModel() {
	// Recalculate new matrix
	translationMatrix = glm::translate(sm_identityMatrix, position);

	rotationMatrix = glm::rotate(sm_identityMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	scaleMatrix = glm::scale(sm_identityMatrix, scale);

	m_modelNormalMatrix = translationMatrix * rotationMatrix;
	m_modelMatrix = m_modelNormalMatrix * scaleMatrix;

	// Send model matrix to shader
	glUniformMatrix4fv(ShaderManager::GetCurrentShader()->GetModelNormalUniformLocation(), 1, GL_FALSE, glm::value_ptr(m_modelNormalMatrix));
	glUniformMatrix4fv(ShaderManager::GetCurrentShader()->GetModelUniformLocation(), 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

	// Bind mesh and all its textures
	m_mesh.BindMesh();
	for (Texture* texture : m_textures) {
		texture->BindTexture();
	}
}