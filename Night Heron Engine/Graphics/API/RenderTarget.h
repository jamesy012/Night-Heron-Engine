#pragma once

#include "GFXObj.h"

class Texture;

class RenderTarget : public GFXObj {
friend class GFX;
protected:
	RenderTarget(unsigned int a_Width, unsigned int a_Height);
public:

	virtual void SetSize(unsigned int a_Width, unsigned int a_Height) = 0;

	virtual void Use() = 0;

	virtual void Reset() = 0;

	virtual Texture* GetTexture() = 0;

	virtual void SetupRenderTarget_Internal() = 0;
protected:

	unsigned int m_Width;
	unsigned int m_Height;

	Texture* m_Texture;
};