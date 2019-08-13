#pragma once

#include "../api/RenderTarget.h"

class RenderTargetDX11 : public RenderTarget {
public:
	RenderTargetDX11(unsigned int a_Width, unsigned int a_Height) : RenderTarget(a_Width, a_Height)
		, m_RenderTarget(nullptr)
		, m_DepthStencil(nullptr)
		, m_DepthStencilTexture(nullptr) {
	};

	virtual void AddBuffer(RenderTargetBufferTypes a_Type, RenderTargetBufferFormats a_Format, RenderTargetBufferSize a_Size) override {};
	virtual void Create() override {};

	// Inherited via RenderTarget
	virtual void SetSize(unsigned int a_Width, unsigned int a_Height) override;
	//virtual void Use() override;
	//virtual void Reset() override;
	virtual void Bind() override;
	virtual Texture* GetTexture() override;

	virtual void SetupRenderTarget_Internal() override;
private:
	virtual void SetDebugObjName_Internal() override;

	struct ID3D11RenderTargetView* m_RenderTarget;
	struct ID3D11DepthStencilView* m_DepthStencil;

	class Texture* m_DepthStencilTexture;
};
