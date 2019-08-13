#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

void Texture::LoadTexture(CMString a_Path) {
	m_Path = a_Path;

	int imageFormat = 0;

	//m_TextureData = stbi_load(a_Path.c_str(), &m_Width, &m_Height, &m_ImageFormat, STBI_default);
	m_TextureData = stbi_load(a_Path.Get(), &m_Width, &m_Height, &imageFormat, STBI_rgb_alpha);
	m_ImageFormat = STBiToSize(4);//seems to be loading it at RBG instead of RGBA

	createData();

	SetDebugObjName_Internal();

	stbi_image_free(m_TextureData);
}

void Texture::CreateTexture(int a_Width, int a_Height) {
	m_Width = a_Width;
	m_Height = a_Height;
	m_ImageFormat = STBiToSize(4);

	if (m_TextureData == nullptr) {
		m_TextureData = new unsigned char[m_Width * m_Height * m_ImageFormat];
		memset(m_TextureData, 255, m_Width * m_Height * m_ImageFormat);
	}

	createData();
}

TextureSizes Texture::STBiToSize(int a_Size) const {
	switch (a_Size) {
		case STBI_grey:
			return TextureSizes::R;
		case STBI_grey_alpha:
			return TextureSizes::RG;
		case STBI_rgb_alpha:
			return TextureSizes::RGBA;
		case STBI_rgb:
		default:
			return TextureSizes::RGB;
	}
}
