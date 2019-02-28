#include "GFXDX11.h"

//#include "WindowDX11.h"
#include "Window.h"

#include <d3d11_1.h>
#include <d3d11_2.h>

#include <imgui-master\imgui.h>
#include <imgui-master\examples\imgui_impl_dx11.h>

#include "ShaderDX11.h"
#include "MeshDX11.h"
#include "TextureDX11.h"

DirectX11Common* GFXDX11::m_CurrentContext = nullptr;

GFXDX11::~GFXDX11() {
	ImGui_ImplDX11_Shutdown();
}

bool GFXDX11::CreateWindowSetUpAPI() {
	m_CurrentGraphics = this;

	//m_Window = new WindowDX11();
	m_Window = new Window();
	m_Window->m_WindowTitle += " - DirectX 11";
	return m_Window->CreateMainWindow() && InitGfx();
}

void GFXDX11::DestroyMainWindow() {
	SwapChain->SetFullscreenState(false, NULL);
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();

	depthStencilView->Release();
	renderTargetView->Release();

	m_Window->DestrowMainWindow();

	delete m_CurrentContext;
	m_CurrentContext = nullptr;
}

void GFXDX11::SwapBuffer() {
	SwapChain->Present(1, 0);
}

void GFXDX11::Clear() {
	const float bgColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3d11DevCon->ClearRenderTargetView(renderTargetView, &m_ClearR);
	d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
	ImGui_ImplDX11_Init(d3d11Device, d3d11DevCon);
}

void GFXDX11::ResizeWindow_Internal(int a_Width, int a_Height) {
	if (SwapChain) {
		HRESULT hr;

		renderTargetView->Release();
		SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D* backbuffer;
		hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
		if (FAILED(hr)) {
			MessageBox(NULL, "Error GetBuffer SwapChain", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		hr = d3d11Device->CreateRenderTargetView(backbuffer, NULL, &renderTargetView);
		backbuffer->Release();
		if (FAILED(hr)) {
			MessageBox(NULL, "Error CreateRenderTargetView", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilBuffer->GetDesc(&depthStencilDesc);

		depthStencilBuffer->Release();
		depthStencilView->Release();

		depthStencilDesc.Width = a_Width;
		depthStencilDesc.Height = a_Height;

		d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
		d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);


		d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);



		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.Width = a_Width;
		viewport.Height = a_Height;
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;

		d3d11DevCon->RSSetViewports(1, &viewport);
	}
}

Shader * GFXDX11::CreateShader() {
	return new ShaderDX11();
}

Mesh * GFXDX11::CreateMesh() {
	return new MeshDX11();
}

Texture * GFXDX11::CreateTexture() {
	return new TextureDX11();
}

RenderTarget * GFXDX11::CreateRenderTarget(int a_Width, int a_Height) {
	return nullptr;
}

ShaderUniformBlock* GFXDX11::CreateBuffer(void * a_Object, unsigned int a_Size) {
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

	hr = d3d11Device->CreateBuffer(&cbbd, &InitData, &sub->m_Buffer);
	if (FAILED(hr)) {
		if (cbbd.ByteWidth % 4 != 0) {
			MessageBox(NULL, "Error Buffer is not a power of 4 bytes", "Error", MB_OK | MB_ICONERROR);
		} else {
			MessageBox(NULL, "Error CreateBuffer cbPerObjectBuffer", "Error", MB_OK | MB_ICONERROR);
		}
	}

	return sub;
}


bool GFXDX11::InitGfx() {

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

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);
	if (FAILED(hr)) {
		MessageBox(NULL, "Error Creating Window", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ID3D11Texture2D* backbuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
	if (FAILED(hr)) {
		MessageBox(NULL, "Error GetBuffer SwapChain", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hr = d3d11Device->CreateRenderTargetView(backbuffer, NULL, &renderTargetView);
	backbuffer->Release();
	if (FAILED(hr)) {
		MessageBox(NULL, "Error CreateRenderTargetView", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

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

	d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.Width = (float)m_Window->m_WindowWidth;
	viewport.Height = (float)m_Window->m_WindowHeight;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	d3d11DevCon->RSSetViewports(1, &viewport);


	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = d3d11Device->CreateRasterizerState(&rasterizerDesc, &rasterState);

	d3d11DevCon->RSSetState(rasterState);


	m_CurrentContext = new DirectX11Common;
	m_CurrentContext->d3d11DevCon = d3d11DevCon;
	m_CurrentContext->d3d11Device = d3d11Device;
	m_CurrentContext->SwapChain = SwapChain;

	return true;
}
