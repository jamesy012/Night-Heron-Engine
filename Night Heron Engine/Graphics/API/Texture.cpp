#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

void Texture::loadTexture(std::string a_Path) {

	m_Path = a_Path;

	//m_TextureData = stbi_load(a_Path.c_str(), &m_Width, &m_Height, &m_ImageFormat, STBI_default);
	m_TextureData = stbi_load(a_Path.c_str(), &m_Width, &m_Height, &m_ImageFormat, STBI_rgb_alpha);
	m_ImageFormat = 4;

	createData();

	stbi_image_free(m_TextureData);
}

void Texture::CreateTexture(int a_Width, int a_Height) {
	m_Width = a_Width;
	m_Height = a_Height;
	m_ImageFormat = 4;

	createData();

}
