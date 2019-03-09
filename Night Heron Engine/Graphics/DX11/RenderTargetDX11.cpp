#include "RenderTargetDX11.h"

#include <d3d11.h>

#include <d3d11.h>
#include <Graphics/DX11/GFXDX11.h>
#include "TextureDX11.h"

// https://stackoverflow.com/questions/13326715/rendertargetview-to-shaderresourceview


void RenderTargetDX11::SetSize(unsigned int a_Width, unsigned int a_Height) {
	//need resize on texture
}

void RenderTargetDX11::Use() {
	GFXDX11::GetCurrentContex()->d3d11DevCon->OMSetRenderTargets(1, &m_RenderTarget, m_DepthStencil);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Width = m_Width;
	viewport.Height = m_Height;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	GFXDX11::GetCurrentContex()->d3d11DevCon->RSSetViewports(1, &viewport);


	GFXDX11::GetCurrentContex()->CurrentBoundRenderTarget = m_RenderTarget;
	GFXDX11::GetCurrentContex()->CurrentBoundDepthStencilView = m_DepthStencil;

}

void RenderTargetDX11::Reset() {
	((GFXDX11*)m_CurrentGraphics)->ResetRenderTarget();
}

void RenderTargetDX11::SetupRenderTarget_Internal() {

	TextureDX11* tex2D;
	m_Texture = tex2D = new TextureDX11();
	tex2D->m_DesiredFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex2D->m_ExtraBindFlags = D3D11_BIND_RENDER_TARGET;
	tex2D->m_ShouldCreateSampler = false;
	tex2D->CreateTexture(m_Width, m_Height);

	TextureDX11* depthStencil;
	m_DepthStencilTexture = depthStencil = new TextureDX11();
	depthStencil->m_DesiredFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencil->m_ExtraBindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencil->m_IsShaderResource = false;
	depthStencil->m_ShouldCreateSampler = false;
	depthStencil->CreateTexture(m_Width, m_Height);


	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	GFXDX11::GetCurrentContex()->d3d11Device->CreateRenderTargetView(tex2D->Get2DTextrueRef(), &renderTargetViewDesc, &m_RenderTarget);

	GFXDX11::GetCurrentContex()->d3d11Device->CreateDepthStencilView(depthStencil->Get2DTextrueRef(), NULL, &m_DepthStencil);
}

Texture * RenderTargetDX11::GetTexture() {
	return m_Texture;
}
