#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

void Texture::LoadTexture(CMString a_Path) {
	m_Path = a_Path;

	//m_TextureData = stbi_load(a_Path.c_str(), &m_Width, &m_Height, &m_ImageFormat, STBI_default);
	m_TextureData = stbi_load(a_Path.Get(), &m_Width, &m_Height, &m_ImageFormat, STBI_rgb_alpha);
	m_ImageFormat = 4;

	createData();

	SetDebugObjName_Internal();

	stbi_image_free(m_TextureData);
}

void Texture::CreateTexture(int a_Width, int a_Height) {
	m_Width = a_Width;
	m_Height = a_Height;
	m_ImageFormat = 4;

	if (m_TextureData == nullptr) {
		m_TextureData = new unsigned char[m_Width * m_Height * m_ImageFormat];
		memset(m_TextureData, 255, m_Width * m_Height * m_ImageFormat);
	}

	createData();
}
