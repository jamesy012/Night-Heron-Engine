#pragma once

#include "GFXObj.h"
#include "SimpleMath.h"

class Texture;

//types of formats that can be assigned to a texture or renderbuffer
enum class RenderTargetBufferSize {
	Size8,
	Size16,
	//Size24,
	Size32
};

//types of formats that can be assigned to a texture or renderbuffer
enum class RenderTargetBufferFormats {
	R,
	RG,
	RGB,
	RGBA,
	DEPTH,
	//STENCIL,
	DEPTH_STENCIL,
};

//types of buffers that can be added to this Framebuffer
//Texture if you want to read it
//RenderBuffer if you dont
enum class RenderTargetBufferTypes {
	TEXTURE,
	RENDERBUFFER
};

class RenderTarget : public GFXObj {
	friend class GFX;
protected:
	RenderTarget(unsigned int a_Width, unsigned int a_Height);
public:

	virtual void SetSize(unsigned int a_Width, unsigned int a_Height) = 0;

	virtual void AddBuffer(RenderTargetBufferTypes a_Type, RenderTargetBufferFormats a_Format, RenderTargetBufferSize a_Size = RenderTargetBufferSize::Size16) = 0;
	//finish's setting up the render target
	virtual void Create() = 0;

	//virtual void Use() = 0;
	virtual void Bind() = 0;

	//virtual void Reset() = 0;

	virtual Texture* GetTexture() = 0;

	virtual void SetupRenderTarget_Internal() = 0;

	uint GetWidth() {
		return m_Width;
	}
	uint GetHeight() {
		return m_Height;
	}
protected:

	uint m_Width;
	uint m_Height;

	Texture* m_Texture;
};
