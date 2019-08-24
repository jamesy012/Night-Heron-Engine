#include "MeshDX11.h"

#include "GFXDX11.h"

#include "Debug.h"

#include <d3d11.h>

MeshDX11::~MeshDX11() {
	if (m_VertexBuffer) {
		m_VertexBuffer->Release();
	}
	if (m_IndexBuffer) {
		m_IndexBuffer->Release();
	}
}

void MeshDX11::Bind() {
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = m_Vertices.Length() * sizeof(MeshVerticesType);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = m_Indices.Length() * sizeof(MeshIndicesType);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexData.pSysMem = m_Vertices.First();
	indexData.pSysMem = m_Indices.First();

	HRESULT hr;
	CMLOG_SCOPED_NAME("Mesh:" + m_DebugName);

	hr = GFXDX11::GetCurrentContex()->m_Device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
	if (FAILED(hr)) {
		CMLOG("vertexData error %ld\n", hr);
		return;
	}
	hr = GFXDX11::GetCurrentContex()->m_Device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
	if (FAILED(hr)) {
		CMLOG("indexData ERROR %ld\n", hr);
		return;
	}

	SetDebugObjName_Internal();
}

void MeshDX11::Draw() {
	const UINT VertexStride = sizeof(Vertex);
	const UINT VertexOffset = 0;

	GFXDX11::GetCurrentContex()->m_DevCon->IASetVertexBuffers(0, 1, &m_VertexBuffer, &VertexStride, &VertexOffset);
	GFXDX11::GetCurrentContex()->m_DevCon->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	GFXDX11::GetCurrentContex()->m_DevCon->DrawIndexed(m_Indices.Length(), 0, 0);
}

void MeshDX11::SetDebugObjName_Internal() {
	if (m_VertexBuffer) {
		CMString temp = m_DebugName + " Vertex Buffer";
		m_VertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	}
	if (m_IndexBuffer) {
		CMString temp = m_DebugName + " Index Buffer";
		m_IndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	}
}
