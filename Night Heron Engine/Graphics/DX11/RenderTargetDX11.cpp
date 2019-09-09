#include "RenderTargetDX11.h"

#include "Debug.h"

#include <d3d11.h>

#include <d3d11.h>
#include <Graphics/DX11/GFXDX11.h>
#include "TextureDX11.h"

// https://stackoverflow.com/questions/13326715/rendertargetview-to-shaderresourceview

void RenderTargetDX11::AddBuffer(RenderTargetBufferTypes a_Type, RenderTargetBufferFormats a_Format, RenderTargetBufferSize a_Size) {

	//temp for shadows
	if (a_Type == RenderTargetBufferTypes::TEXTURE && a_Format == RenderTargetBufferFormats::DEPTH) {

		TextureDX11* tex2D;
		m_Texture = m_DepthStencilTexture = tex2D = new TextureDX11();
		tex2D->m_DesiredFormat = DXGI_FORMAT_R24G8_TYPELESS;
		tex2D->m_DesiredShaderResourceFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		tex2D->m_ExtraBindFlags = D3D11_BIND_DEPTH_STENCIL;
		tex2D->m_ShouldCreateSampler = false;
		tex2D->CreateTexture(m_Width, m_Height);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		GFXDX11::GetCurrentContex()->m_Device->CreateDepthStencilView(tex2D->Get2DTextrueRef(), &depthStencilViewDesc, &m_DepthStencil);


		D3D11_SAMPLER_DESC comparisonSamplerDesc;
		ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		comparisonSamplerDesc.BorderColor[0] = 1.0f;
		comparisonSamplerDesc.BorderColor[1] = 1.0f;
		comparisonSamplerDesc.BorderColor[2] = 1.0f;
		comparisonSamplerDesc.BorderColor[3] = 1.0f;
		comparisonSamplerDesc.MinLOD = 0.f;
		comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		comparisonSamplerDesc.MipLODBias = 0.f;
		comparisonSamplerDesc.MaxAnisotropy = 0;
		comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

		ID3D11SamplerState* samplerRef = tex2D->GetSamplerRef();

		GFXDX11::GetCurrentContex()->m_Device->CreateSamplerState(&comparisonSamplerDesc, &samplerRef);

		tex2D->SetSamplerRef(samplerRef);
	}
}

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

void RenderTargetDX11::Blitz(RenderTarget * a_From, uint a_FromX, uint a_FromY, uint a_FromWidth, uint a_FromHeight, uint a_ToX, uint a_ToY, uint a_ToWidth, uint a_ToHeight, RenderTargetBlitzFormats a_Format) {
	CMASSERT(a_FromWidth != a_ToWidth);
	CMASSERT(a_FromHeight != a_ToHeight);

	D3D11_BOX sourceRegion;
	sourceRegion.left = a_FromX;
	sourceRegion.right = a_FromWidth;
	sourceRegion.top = a_FromY;
	sourceRegion.bottom = a_FromHeight;
	sourceRegion.front = 0;
	sourceRegion.back = 1;

	//GFXDX11::GetCurrentContex()->m_DevCon->CopySubresourceRegion((ID3D11Resource*)m_Texture->getTexturePtr(), 0, a_ToX, a_ToY, 0, (ID3D11Resource*)a_From->GetTexture()->getTexturePtr(), 0, &sourceRegion);

}
