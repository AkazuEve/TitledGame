#include "ShaderManager.hpp"

extern Shader* currentShader;

ShaderManager::ShaderManager() {

}

ShaderManager::~ShaderManager() {

}

void ShaderManager::OnUIRender() {
	ImGui::Begin("Shaders");

	// Cant forget this
	//ImGui::Image(reinterpret_cast<ImTextureID>(m_curentBuffer), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

	for (Shader* shader : m_shaders) {
		if (ImGui::TreeNode(shader->name.c_str())) {
			if (ImGui::Button("Recompile")) {
				shader->RecompileShader();
			}
			if (ImGui::Button("Set as default")) {
				currentShader = shader;
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
}