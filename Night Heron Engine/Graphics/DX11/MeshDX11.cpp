#include "MeshDX11.h"

#include "GFXDX11.h"

#include <d3d11.h>


void MeshDX11::Bind() {

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = m_Vertices.size() * sizeof(MeshVerticesType);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = m_Indices.size() * sizeof(MeshIndicesType);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexData.pSysMem = &m_Vertices.front();
	indexData.pSysMem = &m_Indices.front();

	
	HRESULT hr;

	hr = GFXDX11::GetCurrentContex()->d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexData, &squareVertBuffer);
	if (FAILED(hr)) {
		printf("vertexData error %ld\n", hr);
		return;
	}
	hr = GFXDX11::GetCurrentContex()->d3d11Device->CreateBuffer(&indexBufferDesc, &indexData, &squareIndexBuffer);
	if (FAILED(hr)) {
		printf("indexData ERROR %ld\n", hr);
		return;
	}
}

void MeshDX11::Draw() {

	const UINT VertexStride = sizeof(Vertex);
	const UINT VertexOffset = 0;

	GFXDX11::GetCurrentContex()->d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &VertexStride, &VertexOffset);
	GFXDX11::GetCurrentContex()->d3d11DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	GFXDX11::GetCurrentContex()->d3d11DevCon->DrawIndexed(m_Indices.size(), 0, 0);
}
