#pragma once

#include "GFXObj.h"
#include "SimpleMath.h"

class Texture;

class RenderTarget : public GFXObj {
	friend class GFX;
protected:
	RenderTarget(unsigned int a_Width, unsigned int a_Height);
public:

	virtual void SetSize(unsigned int a_Width, unsigned int a_Height) = 0;

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
