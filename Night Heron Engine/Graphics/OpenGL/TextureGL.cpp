#include "TextureGL.h"

#include <GL/glew.h>
#include <stb-master/stb_image.h>

static unsigned int getGLFormatFromSTBFormat(unsigned int a_Format) {
	switch (a_Format) {
		case STBI_rgb_alpha:
			return GL_RGBA;
		case STBI_rgb:
		default:
			return GL_RGB;
	}
}

void TextureGL::createData() {
	unsigned int glType = getGLFormatFromSTBFormat(m_ImageFormat);

	glGenTextures(1, &m_BufferID);
	glBindTexture(GL_TEXTURE_2D, m_BufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, glType, m_Width, m_Height, 0, glType, GL_UNSIGNED_BYTE, m_TextureData);

	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

	glBindTexture(GL_TEXTURE_2D, 0);
}
