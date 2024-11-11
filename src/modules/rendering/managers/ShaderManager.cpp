#include "ShaderManager.hpp"

Shader* ShaderManager::m_currentShader = nullptr;
std::vector<Shader*> ShaderManager::m_shaders{};

Shader* ShaderManager::CreateShader(std::string name, std::string vertexFile, std::string fragmentFile) {
	Shader* shader = new Shader(name, vertexFile, fragmentFile);
	m_currentShader = shader;
	m_shaders.push_back(shader);

	DEBUGPRINT("ShaderManager:: Created shader:" << shader->name << ", " << shader);

	return shader;
}

void ShaderManager::SetupUI() {
	ImGuiUIManager::AddUIFunction(RenderUI);
}

void ShaderManager::FreeMemory() {
	for (Shader* shader : m_shaders) {
		DEBUGPRINT("ShaderManager:: Deleted shader:" << shader->name << ", " << shader);
		delete(shader);
	}
}

Shader* ShaderManager::GetCurrentShader() { return m_currentShader; }

void ShaderManager::RenderUI() {
	ImGui::Begin("Shaders");

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
						DEBUGPRINT("ImGuiUI:: Deleted shader: " << shader);
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