#pragma once

#include "..\API\GFXAPI.h"

struct DirectX11Common {
	struct IDXGISwapChain* SwapChain;//backbuffer
	struct ID3D11Device* d3d11Device;//dx11 non rendering stuff
	struct ID3D11DeviceContext* d3d11DevCon;//dx11 rendering stuff

	struct ID3D11RenderTargetView* CurrentBoundRenderTarget;
	struct ID3D11DepthStencilView* CurrentBoundDepthStencilView;

};

class GFXDX11 : public GFX {
private:
	static DirectX11Common* m_CurrentContext;
public:
	~GFXDX11();

	static inline DirectX11Common* GetCurrentContex() {
		return m_CurrentContext;
	};

	// Inherited via GFX
	virtual bool CreateWindowSetUpAPI() override;
	virtual void DestroyMainWindow() override;
	virtual void SwapBuffer() override;
	virtual void Clear() override;
	virtual void SetClearColor(float R, float G, float B, float A) override;

	virtual void ImGuiDraw() override;
	virtual void ImGuiNewFrame() override;


	virtual void ResizeWindow_Internal(int a_Width, int a_Height) override;


	virtual class Shader* CreateShader() override;
	virtual class Mesh* CreateMesh() override;
	virtual class Texture* CreateTexture() override;
	virtual class RenderTarget* CreateRenderTarget(int a_Width, int a_Height) override;


	virtual ShaderUniformBlock* CreateBuffer(void * a_Object, unsigned int a_Size) override;

	virtual void PushDebugGroup(CMString a_Name) override;
	virtual void PopDebugGroup() override;

	virtual SimpleVec2 GetImGuiImageUV0() override {
		return SimpleVec2(0, 0);
	};
	virtual SimpleVec2 GetImGuiImageUV1() override {
		return SimpleVec2(1, 1);
	};

	void ResetRenderTarget();

protected:
	virtual void InitImGui_Internal() override;


private:
	bool InitGfx() override;

	struct IDXGISwapChain* SwapChain;//backbuffer
	struct ID3D11Device* d3d11Device;//dx11 non rendering stuff
	struct ID3D11DeviceContext* d3d11DevCon;//dx11 rendering stuff
	struct ID3D11RenderTargetView* renderTargetView;//main buffer
	struct ID3D11DepthStencilView* depthStencilView;
	struct ID3D11Texture2D* depthStencilBuffer;
	struct ID3D11RasterizerState* rasterState;
	struct ID3DUserDefinedAnnotation* pPerf;
	struct ID3D11Debug* pDebug;

};
