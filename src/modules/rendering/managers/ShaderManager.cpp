#include "ShaderManager.hpp"

Shader* ShaderManager::m_currentShader = nullptr;

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
	for (Shader* shader : m_shaders) {
		delete(shader);
	}
}

Shader* ShaderManager::CreateShader(std::string name, std::string vertexFile, std::string fragmentFile) {
	Shader* shader = new Shader(name, vertexFile, fragmentFile);
	m_currentShader = shader;
	m_shaders.push_back(shader);

	return shader;
}

Shader* ShaderManager::GetCurrentShader() { return m_currentShader; }

void ShaderManager::OnUIRender() {
	ImGui::Begin("Shaders");

	// Cant forget this
	//ImGui::Image(reinterpret_cast<ImTextureID>(m_curentBuffer), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

	for (Shader* shader : m_shaders) {
		if (ImGui::TreeNode(shader->name.c_str())) {
			if (ImGui::Button("Recompile")) {
				shader->RecompileShader();
			}
			if (m_shaders.size() > 1) {

				if (ImGui::Button("Set as default")) {
					m_currentShader = shader;
				}

				if (ImGui::Button("Delete")) {
					// Remove shader pointer from shader pointer vector
					std::vector<Shader*>::iterator position = std::find(m_shaders.begin(), m_shaders.end(), shader);
					if (position != m_shaders.end()) {
						m_shaders.erase(position);
						delete(shader);
						DEBUGPRINT("Removed Shader from shader list: " << this);
						if (m_currentShader == shader)
							m_currentShader = m_shaders[0];
					}
				}
			};
			ImGui::TreePop();
		}
	}

	ImGui::End();
}