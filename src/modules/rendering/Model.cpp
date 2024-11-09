#include "Model.hpp"

extern Shader* currentShader;

glm::mat4 Model::sm_identityMatrix{ 1.0f };
std::vector<Model*> Model::m_models;

Model::Model() {
	// Push created model pointer to vector
	m_models.push_back(this);
	DEBUGPRINT("Created model: " << this);
}

Model::~Model() {
	// Destroy all allocated textures
	for (Texture* texture : m_textures) {
		delete(texture);
	}

	// Remove this pointer from models vector
	std::vector<Model*>::iterator position = std::find(m_models.begin(), m_models.end(), this);
	if (position != m_models.end()) {
		m_models.erase(position);
		DEBUGPRINT("Removed Model from model list: " << this);
	}
}

void Model::AddMesh(std::string name, const MeshData& data, GLenum indexFormat) {
	this->name = name;
	m_mesh.LoadMeshData(data, indexFormat);
}

void Model::AddTexture(GLenum textureSlot, std::string textuprePath) {
	// Create new texture
	Texture* tmp = new Texture();

	// Setup data and push to texture vector
	tmp->LoadTextureData(textuprePath, textureSlot);
	m_textures.push_back(tmp);
}

void Model::BindModel() {
	// Recalculate new matrix
	static glm::mat4 translationMatrix{ 1.0f };
	static glm::mat4 rotationMatrix{ 1.0f };
	static glm::mat4 scaleMatrix{ 1.0f };

	translationMatrix = glm::translate(sm_identityMatrix, position);

	rotationMatrix = glm::rotate(sm_identityMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	scaleMatrix = glm::scale(sm_identityMatrix, scale);

	m_modelNormalMatrix = translationMatrix * rotationMatrix;
	m_modelMatrix = m_modelNormalMatrix * scaleMatrix;

	// Send model matrix to shader
	glUniformMatrix4fv(currentShader->GetModelNormalUniformLocation(), 1, GL_FALSE, glm::value_ptr(m_modelNormalMatrix));
	glUniformMatrix4fv(currentShader->GetModelUniformLocation(), 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

	// Bind mesh and all its textures
	m_mesh.BindMesh();
	for (Texture* texture : m_textures) {
		texture->BindTexture();
	}
}