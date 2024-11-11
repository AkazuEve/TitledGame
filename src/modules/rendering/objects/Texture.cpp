#include "Texture.hpp"

#include "../../resources/ResourceManager.hpp"

Texture::Texture() {
	// Generate texture object
	glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_texture);
}

void Texture::LoadTextureData(std::string texturePath, GLenum textureSlot) {
	m_textureSlot = textureSlot;

	int x, y;
	
	unsigned char* textureData = ResourceManager::LoadTexture(texturePath.c_str(), &x, &y);

	// Send data to GPU
	glTextureStorage2D(m_texture, 5, GL_RGBA8, x, y);
	glTextureSubImage2D(m_texture, 0, 0, 0, x, y, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	// Generate mipmaps
	glGenerateTextureMipmap(m_texture);
}

void Texture::BindTexture() {
	glBindTextureUnit(m_textureSlot - GL_TEXTURE0, m_texture);
}