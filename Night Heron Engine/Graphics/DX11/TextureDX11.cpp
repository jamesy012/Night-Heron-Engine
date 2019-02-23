#include "TextureDX11.h"

#include <d3d11.h>
#include <Graphics/DX11/GFXDX11.h>
#include <stb-master/stb_image.h>

static DXGI_FORMAT getDX11FormatFromSTBFormat(unsigned int a_Format) {
	switch (a_Format) {
		case STBI_rgb_alpha:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case STBI_rgb:
		default:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}


void TextureDX11::bind(unsigned int a_Slot) {
	GFXDX11::GetCurrentContex()->d3d11DevCon->PSSetSamplers(a_Slot, 1, &m_SamplerRef);
	GFXDX11::GetCurrentContex()->d3d11DevCon->PSSetShaderResources(a_Slot, 1, &m_TextureSRV);
}

void TextureDX11::createData() {
	HRESULT hr;
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Format = getDX11FormatFromSTBFormat(m_ImageFormat);
	texDesc.Width = m_Width;
	texDesc.Height = m_Height;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA resources;
	ZeroMemory(&resources, sizeof(D3D11_SUBRESOURCE_DATA));
	resources.pSysMem = m_TextureData;
	resources.SysMemPitch = m_Width * m_ImageFormat;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = texDesc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.TextureCube.MipLevels = texDesc.MipLevels;
	srvd.TextureCube.MostDetailedMip = 0;


	hr = GFXDX11::GetCurrentContex()->d3d11Device->CreateTexture2D(&texDesc, &resources, &m_TextureRef);

	hr = GFXDX11::GetCurrentContex()->d3d11Device->CreateShaderResourceView(m_TextureRef, &srvd, &m_TextureSRV);


	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = GFXDX11::GetCurrentContex()->d3d11Device->CreateSamplerState(&samplerDesc, &m_SamplerRef);

}
