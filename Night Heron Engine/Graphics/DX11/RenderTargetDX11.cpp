#include "RenderTargetDX11.h"

#include <d3d11.h>

#include <d3d11.h>
#include <Graphics/DX11/GFXDX11.h>
#include "TextureDX11.h"

// https://stackoverflow.com/questions/13326715/rendertargetview-to-shaderresourceview

void RenderTargetDX11::SetSize(unsigned int a_Width, unsigned int a_Height) {
	//need resize on texture
}
/*
void RenderTargetDX11::Use() {
	GFXDX11::GetCurrentContex()->m_DevCon->OMSetRenderTargets(1, &m_RenderTarget, m_DepthStencil);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Width = m_Width;
	viewport.Height = m_Height;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	GFXDX11::GetCurrentContex()->m_DevCon->RSSetViewports(1, &viewport);

	GFXDX11::GetCurrentContex()->m_CurrentBoundRenderTarget = m_RenderTarget;
	GFXDX11::GetCurrentContex()->m_CurrentBoundDepthStencilView = m_DepthStencil;
}

void RenderTargetDX11::Reset() {
	((GFXDX11*)_CGraphics)->ResetRenderTarget();
}
*/

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

	GFXDX11::GetCurrentContex()->m_Device->CreateRenderTargetView(tex2D->Get2DTextrueRef(), &renderTargetViewDesc, &m_RenderTarget);

	GFXDX11::GetCurrentContex()->m_Device->CreateDepthStencilView(depthStencil->Get2DTextrueRef(), NULL, &m_DepthStencil);
}

void RenderTargetDX11::SetDebugObjName_Internal() {
	if (m_Texture) {
		m_Texture->SetDebugObjName(m_DebugName + " Texture");
	}
	if (m_DepthStencilTexture) {
		m_DepthStencilTexture->SetDebugObjName(m_DebugName + " Depth Texture");
	}
	if (m_RenderTarget) {
		CMString temp = m_DebugName + " Render Target View";
		m_RenderTarget->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	}
	if (m_DepthStencil) {
		CMString temp = m_DebugName + " Depth Stencil View";
		m_DepthStencil->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	}
}

void RenderTargetDX11::Bind() {
	GFXDX11::GetCurrentContex()->m_DevCon->OMSetRenderTargets(1, &m_RenderTarget, m_DepthStencil);
	GFXDX11::GetCurrentContex()->m_CurrentBoundRenderTarget = m_RenderTarget;
	GFXDX11::GetCurrentContex()->m_CurrentBoundDepthStencilView = m_DepthStencil;
}

Texture* RenderTargetDX11::GetTexture() {
	return m_Texture;
}
