#include "RenderTargetGL.h"

#include <GL/glew.h>

#include "TextureGL.h"
#include "GFXOpenGL.h"
#include "Window.h"

#include "Debug.h"

void RenderTargetGL::AddBuffer(RenderTargetBufferTypes a_Type, RenderTargetBufferFormats a_Format, RenderTargetBufferSize a_Size) {
	CMASSERT_MSG(a_Type != RenderTargetBufferTypes::TEXTURE, "Buffer not supported");
	const uint GLsize = GetGLFormatSize(a_Format, a_Size);
}

void RenderTargetGL::Create() {
}

void RenderTargetGL::SetSize(unsigned int a_Width, unsigned int a_Height) {
	//need resize on texture
}

/*
void RenderTargetGL::Use() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glViewport(0, 0, m_Width, m_Height);
}

void RenderTargetGL::Reset() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, _CGraphics->m_Window->m_WindowWidth, _CGraphics->m_Window->m_WindowHeight);
}
*/

void RenderTargetGL::SetupRenderTarget_Internal() {
	//(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGBA, 16);
	//return GL_RGBA16;
	m_Texture = new TextureGL();
	m_Texture->CreateTexture(m_Width, m_Height);

	//create renderbuffer
	glGenRenderbuffers(1, &m_RenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
	//set up renderbuffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);

	//unbind renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, (int)m_Texture->getTexturePtr(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);

	unsigned int drawBuffers[] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer Error ID: %u\n", status);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//done, lets unbind the buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargetGL::SetDebugObjName_Internal() {
	if (m_Texture) {
		m_Texture->SetDebugObjName(m_DebugName + " Texture");
	}
	if (m_Fbo) {
		CMString temp = m_DebugName + " Frame Buffer";
		glObjectLabel(GL_FRAMEBUFFER, m_Fbo, -1, temp.Get());
	}
	if (m_RenderBuffer) {
		CMString temp = m_DebugName + " Render Buffer View";
		glObjectLabel(GL_RENDERBUFFER, m_RenderBuffer, -1, temp.Get());
	}
}

void RenderTargetGL::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}

Texture* RenderTargetGL::GetTexture() {
	return m_Texture;
}

//converts a_Format and a_FormatSize into their opengl counterparts
//eg a_Format = RGB, a_FormatSize = 16
//result GL_RGB16 (0x8054)
uint RenderTargetGL::GetGLFormatSize(RenderTargetBufferFormats a_Format, RenderTargetBufferSize a_FormatSize) {
	switch (a_Format) {
		case RenderTargetBufferFormats::R:
			{
				switch (a_FormatSize) {
					case RenderTargetBufferSize::Size8:
						return GL_R8;
					case RenderTargetBufferSize::Size16:
						return GL_R16;
				}
				CMASSERT_MSG(true, L"RenderTargetBufferFormats::R does not have a_FormatSize");
			}
		case RenderTargetBufferFormats::RG:
			{
				switch (a_FormatSize) {
					case RenderTargetBufferSize::Size8:
						return GL_RG8;
					case RenderTargetBufferSize::Size16:
						return GL_RG16;
				}
				CMASSERT_MSG(true, L"FrameRenderTargetBufferFormatsbufferBufferFormats::RG does not have a_FormatSize");
			}
		case RenderTargetBufferFormats::RGB:
			{
				switch (a_FormatSize) {
					//case 4:
					//	return GL_RGB4;
					//case 5:
					//	return GL_RGB5;
					case RenderTargetBufferSize::Size8:
						return GL_RGB8;
					//case 10:
					//	return GL_RGB10;
					//case 12:
					//	return GL_RGB12;
					case RenderTargetBufferSize::Size16:
						return GL_RGB16;
				}
				CMASSERT_MSG(true, L"RenderTargetBufferFormats::RGB does not have a_FormatSize");
			}
		case RenderTargetBufferFormats::RGBA:
			{
				switch (a_FormatSize) {
					//case 2:
					//	return GL_RGBA2;
					//case 4:
					//	return GL_RGBA4;
					case RenderTargetBufferSize::Size8:
						return GL_RGBA8;
					//case 12:
					//	return GL_RGBA12;
					case RenderTargetBufferSize::Size16:
						return GL_RGBA16;
				}
				CMASSERT_MSG(true, L"RenderTargetBufferFormats::RGBA does not have a_FormatSize");
			}
		case RenderTargetBufferFormats::DEPTH:
			{
				switch (a_FormatSize) {
					case RenderTargetBufferSize::Size16:
						return GL_DEPTH_COMPONENT16;
					//case 24:
					//	return GL_DEPTH_COMPONENT24;
					case RenderTargetBufferSize::Size32:
						return GL_DEPTH_COMPONENT32;
				}
				CMASSERT_MSG(true, L"RenderTargetBufferFormats::DEPTH does not have a_FormatSize");
			}
		//case RenderTargetBufferFormats::STENCIL:
		//	return GL_STENCIL;
		case RenderTargetBufferFormats::DEPTH_STENCIL:
			return GL_DEPTH_STENCIL;
	}
	return -1;
}
