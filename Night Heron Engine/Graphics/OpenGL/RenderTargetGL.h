#pragma once

#include "..\API\RenderTarget.h"

class RenderTargetGL : public RenderTarget {
public:
	RenderTargetGL(unsigned int a_Width, unsigned int a_Height) : RenderTarget(a_Width, a_Height) {
	};

	virtual void AddBuffer(RenderTargetBufferTypes a_Type, RenderTargetBufferFormats a_Format, RenderTargetBufferSize a_Size) override;
	virtual void Create() override;

	// Inherited via RenderTarget
	virtual void SetSize(unsigned int a_Width, unsigned int a_Height) override;
	//virtual void Use() override;
	//virtual void Reset() override;
	virtual void Bind() override;
	virtual Texture* GetTexture() override;

	virtual void SetupRenderTarget_Internal() override;

private:
	virtual void SetDebugObjName_Internal() override;

	uint GetGLFormatSize(RenderTargetBufferFormats a_Format, RenderTargetBufferSize a_FormatSize);

	unsigned int m_Fbo = 0;
	unsigned int m_RenderBuffer = 0;
};
