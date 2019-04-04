#include "RenderTargetGL.h"

#include <GL/glew.h>

#include "TextureGL.h"
#include "GFXOpenGL.h"
#include "Window.h"

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
