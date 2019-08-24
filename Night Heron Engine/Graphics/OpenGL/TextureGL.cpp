#include "TextureGL.h"

#include <GL/glew.h>
#include <stb-master/stb_image.h>

static unsigned int getGLFormatFromSize(TextureSizes a_Format) {
	switch (a_Format) {
		case TextureSizes::R:
			return GL_R;
		case TextureSizes::RG:
			return GL_RG;
		case TextureSizes::RGBA:
			return GL_RGBA;
		case TextureSizes::RGB:
		default:
			return GL_RGB;
	}
}

void TextureGL::SetDebugObjName_Internal() {
	if (m_BufferID) {
		glObjectLabel(GL_TEXTURE, m_BufferID, -1, m_DebugName.Get());
	}
}

void TextureGL::createData() {
	unsigned int glType = getGLFormatFromSize(m_ImageFormat);

	glGenTextures(1, &m_BufferID);
	glBindTexture(GL_TEXTURE_2D, m_BufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureData);

	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

	glBindTexture(GL_TEXTURE_2D, 0);
}
