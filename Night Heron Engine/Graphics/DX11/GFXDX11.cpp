#include "GFXDX11.h"

//#include "WindowDX11.h"
#include "Window.h"

#include <d3d11_2.h>
//#include <d3d9.h>

#include <Dependency/ImGui/imgui.h>
#include <Dependency/ImGui/examples/imgui_impl_dx11.h>

#include "ShaderDX11.h"
#include "MeshDX11.h"
#include "TextureDX11.h"
#include "RenderTargetDX11.h"
#include "Singletons.h"

DirectX11Common* GFXDX11::m_CurrentContext = nullptr;

GFXDX11::GFXDX11() {
	m_Window = new Window();
	m_Window->m_WindowTitle += " - DirectX 11";
}

GFXDX11::~GFXDX11() {
	ImGui_ImplDX11_Shutdown();

	delete m_Window;
}

bool GFXDX11::CreateWindowSetUpAPI() {
	_CGraphics = this;

	return m_Window->CreateMainWindow() && InitGfx();
}

void GFXDX11::DestroyMainWindow() {
	m_SwapChain->SetFullscreenState(false, NULL);
	m_SwapChain->Release();
	m_Device->Release();
	m_DevCon->Release();

	m_DepthStencilView->Release();
	m_RenderTargetView->Release();

	m_Window->DestrowMainWindow();

	delete m_CurrentContext;
	m_CurrentContext = nullptr;
}

void GFXDX11::SwapBuffer() {
	m_SwapChain->Present(1, 0);
}

void GFXDX11::Clear() {
	m_DevCon->ClearRenderTargetView(m_CurrentContext->m_CurrentBoundRenderTarget, &m_ClearR);
	m_DevCon->ClearDepthStencilView(m_CurrentContext->m_CurrentBoundDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GFXDX11::SetClearColor(float R, float G, float B, float A) {
	GFX::SetClearColor(R, G, B, A);
}

void GFXDX11::ImGuiDraw() {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GFXDX11::ImGuiNewFrame() {
	ImGui_ImplDX11_NewFrame();
	m_Window->ImGuiNewFrame();
	ImGui::NewFrame();
}

void GFXDX11::InitImGui_Internal() {
	ImGui_ImplDX11_Init(m_Device, m_DevCon);
}

void GFXDX11::ResizeWindow_Internal(int a_Width, int a_Height) {
	if (m_SwapChain) {
		HRESULT hr;

		m_RenderTargetView->Release();
		m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D* backbuffer;
		hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& backbuffer);
		if (FAILED(hr)) {
			MessageBox(NULL, "Error GetBuffer SwapChain", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		hr = m_Device->CreateRenderTargetView(backbuffer, NULL, &m_RenderTargetView);
		backbuffer->Release();
		if (FAILED(hr)) {
			MessageBox(NULL, "Error CreateRenderTargetView", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		m_DepthStencilBuffer->GetDesc(&depthStencilDesc);

		m_DepthStencilBuffer->Release();
		m_DepthStencilView->Release();

		depthStencilDesc.Width = a_Width;
		depthStencilDesc.Height = a_Height;

		m_Device->CreateTexture2D(&depthStencilDesc, NULL, &m_DepthStencilBuffer);
		m_Device->CreateDepthStencilView(m_DepthStencilBuffer, NULL, &m_DepthStencilView);

		m_DevCon->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.Width = (float)a_Width;
		viewport.Height = (float)a_Height;
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;

		m_DevCon->RSSetViewports(1, &viewport);

		CMString temp;
		temp = "DX11 Main View";
		m_RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
		temp = "DX11 Depth Stencil View";
		m_DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
		temp = "DX11 Depth Stencil Buffer";
		m_DepthStencilBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
		temp = "DX11 Back Buffer";
		backbuffer->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	}
}

void GFXDX11::SetFpsLimit(int a_NewLimit) {
	m_FpsLimit = a_NewLimit;
}

Shader* GFXDX11::CreateShader() {
	return new ShaderDX11();
}

Mesh* GFXDX11::CreateMesh() {
	return new MeshDX11();
}

Texture* GFXDX11::CreateTexture() {
	return new TextureDX11();
}

RenderTarget* GFXDX11::CreateRenderTarget(int a_Width, int a_Height) {
	return new RenderTargetDX11(a_Width, a_Height);
}

ShaderUniformBlock* GFXDX11::CreateBuffer(void* a_Object, unsigned int a_Size) {
	HRESULT hr;
	ShaderUniformBlockDX11* sub = new ShaderUniformBlockDX11();
	sub->m_Size = a_Size;

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DYNAMIC;
	cbbd.ByteWidth = a_Size;
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &a_Object;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	hr = m_Device->CreateBuffer(&cbbd, &InitData, &sub->m_Buffer);
	if (FAILED(hr)) {
		if (cbbd.ByteWidth % 4 != 0) {
			MessageBox(NULL, "Error Buffer is not a power of 4 bytes", "Error", MB_OK | MB_ICONERROR);
		} else {
			MessageBox(NULL, "Error CreateBuffer cbPerObjectBuffer", "Error", MB_OK | MB_ICONERROR);
		}
	}

	sub->UpdateBuffer(a_Object);

	return sub;
}

void GFXDX11::BindTexture(Texture * a_Tex, uint a_Slot) {
	const TextureDX11* dxTex = (TextureDX11*)a_Tex;
	ID3D11ShaderResourceView* textureSRV = dxTex->GetTextureSRV();
	ID3D11SamplerState* samplerRef = dxTex->GetSamplerRef();
	GFXDX11::GetCurrentContex()->m_DevCon->PSSetSamplers(a_Slot, 1, &samplerRef);
	GFXDX11::GetCurrentContex()->m_DevCon->PSSetShaderResources(a_Slot, 1, &textureSRV);
	m_TextureSlots[a_Slot] = a_Tex;
}

void GFXDX11::UnbindTexture(uint a_Slot) {
	ID3D11SamplerState* nullHolderSampler[1] = { NULL };
	ID3D11ShaderResourceView* nullHolderSRV[1] = { NULL };
	GFXDX11::GetCurrentContex()->m_DevCon->PSSetSamplers(a_Slot, 1, nullHolderSampler);
	GFXDX11::GetCurrentContex()->m_DevCon->PSSetShaderResources(a_Slot, 1, nullHolderSRV);
	m_TextureSlots[a_Slot] = nullptr;
}

void GFXDX11::PushDebugGroup(CMString a_Name) {
	std::wstring stemp = std::wstring(a_Name.begin(), a_Name.end());
	if (m_PerfDebug) {
		m_PerfDebug->BeginEvent(stemp.c_str());
	} else {
		//D3DPERF_BeginEvent(D3DCOLOR_RGBA(255, 0, 255, 0), stemp.c_str());
	}
}

void GFXDX11::PopDebugGroup() {
	if (m_PerfDebug) {
		m_PerfDebug->EndEvent();
	} else {
		//D3DPERF_EndEvent();
	}
}

void GFXDX11::UseRenderTarget(RenderTarget * a_Rt) {
	a_Rt->Bind();

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Width = (float)a_Rt->GetWidth();
	viewport.Height = (float)a_Rt->GetHeight();
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	GFXDX11::GetCurrentContex()->m_DevCon->RSSetViewports(1, &viewport);
}

void GFXDX11::ResetRenderTarget() {
	if (!m_DevCon) {
		return;
	}
	m_DevCon->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Width = (float)m_Window->m_WindowWidth;
	viewport.Height = (float)m_Window->m_WindowHeight;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	m_DevCon->RSSetViewports(1, &viewport);

	m_CurrentContext->m_CurrentBoundRenderTarget = m_RenderTargetView;
	m_CurrentContext->m_CurrentBoundDepthStencilView = m_DepthStencilView;
}

void GFXDX11::ResetShader() {
	_CCurrentShader = nullptr;
	m_DevCon->IASetInputLayout(nullptr);

	m_DevCon->VSSetShader(nullptr, 0, 0);
	m_DevCon->PSSetShader(nullptr, 0, 0);
}

bool GFXDX11::InitGfx() {
	m_CurrentContext = new DirectX11Common();
	HRESULT hr;

	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = m_Window->m_WindowWidth;
	bufferDesc.Height = m_Window->m_WindowHeight;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = m_Window->m_HWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DevCon);
	if (FAILED(hr)) {
		MessageBox(NULL, "Error Creating Window", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	ID3D11Texture2D* backbuffer;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& backbuffer);
	if (FAILED(hr)) {
		MessageBox(NULL, "Error GetBuffer SwapChain", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hr = m_Device->CreateRenderTargetView(backbuffer, NULL, &m_RenderTargetView);
	backbuffer->Release();
	if (FAILED(hr)) {
		MessageBox(NULL, "Error CreateRenderTargetView", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hr = m_DevCon->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)& m_PerfDebug);
	hr = m_Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_DebugContext));

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilDesc.Width = m_Window->m_WindowWidth;
	depthStencilDesc.Height = m_Window->m_WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	m_Device->CreateTexture2D(&depthStencilDesc, NULL, &m_DepthStencilBuffer);
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, NULL, &m_DepthStencilView);

	ResetRenderTarget();

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = m_Device->CreateRasterizerState(&rasterizerDesc, &m_MainRasterState);

	m_DevCon->RSSetState(m_MainRasterState);

	m_CurrentContext->m_DevCon = m_DevCon;
	m_CurrentContext->m_Device = m_Device;

	CMString temp;
	temp = "DX11 Main View";
	m_RenderTargetView->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	temp = "DX11 Depth Stencil View";
	m_DepthStencilView->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	temp = "DX11 Depth Stencil Buffer";
	m_DepthStencilBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());
	temp = "DX11 Back Buffer";
	backbuffer->SetPrivateData(WKPDID_D3DDebugObjectName, temp.Size(), temp.Get());

	return true;
}
