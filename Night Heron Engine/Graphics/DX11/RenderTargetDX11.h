#pragma once

#include "../api/RenderTarget.h"

class RenderTargetDX11 : public RenderTarget {
public:
	RenderTargetDX11(unsigned int a_Width, unsigned int a_Height) : RenderTarget(a_Width, a_Height) {
	};

	// Inherited via RenderTarget
	virtual void SetSize(unsigned int a_Width, unsigned int a_Height) override;
	virtual void Use() override;
	virtual void Reset() override;
	virtual Texture* GetTexture() override;

	virtual void SetupRenderTarget_Internal() override;
private:
	struct ID3D11RenderTargetView* m_RenderTarget;
	struct ID3D11DepthStencilView* m_DepthStencil;

	class Texture* m_DepthStencilTexture;
};
