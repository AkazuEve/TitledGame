#pragma once

#include <string>

#include "../Debug.hpp"

// Glad for OpenGL functions
#include "../../ext/source/glad/glad.h"

#include "../resources/ResourceManager.hpp"

class Texture {
public:
	// Creates OpenGL objects
	Texture();
	Texture(const Texture&) = delete;
	~Texture();

	// Sends the data to GPU and sets used texture slot
	void LoadTextureData(std::string texturePath, GLenum textureSlot);

	// Binds the texture
	void BindTexture();

private:
	GLuint m_texture{ 0 };
	GLenum m_textureSlot{ 0 };
};