#pragma once

#include "..\API\GFXAPI.h"

struct DirectX11Common {
	struct ID3D11Device* m_Device;//dx11 non rendering stuff
	struct ID3D11DeviceContext* m_DevCon;//dx11 rendering stuff

	struct ID3D11RenderTargetView* m_CurrentBoundRenderTarget;
	struct ID3D11DepthStencilView* m_CurrentBoundDepthStencilView;
};

class GFXDX11 : public GFX {
private:
	static DirectX11Common* m_CurrentContext;
public:
	GFXDX11();
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
	virtual void SetFpsLimit(int a_NewLimit) override;

	virtual class Shader* CreateShader() override;
	virtual class Mesh* CreateMesh() override;
	virtual class Texture* CreateTexture() override;
	virtual class RenderTarget* CreateRenderTarget(int a_Width, int a_Height) override;

	virtual void UseRenderTarget(RenderTarget* a_Rt) override;
	virtual void ResetRenderTarget() override;
	virtual void ResetShader() override;

	virtual ShaderUniformBlock* CreateBuffer(void* a_Object, unsigned int a_Size) override;

	virtual void BindTexture(Texture* a_Tex, uint a_Slot) override;
	virtual void UnbindTexture(uint a_Slot) override;

	virtual void PushDebugGroup(CMString a_Name) override;
	virtual void PopDebugGroup() override;

	virtual SimpleVec2 GetImGuiImageUV0() override {
		return SimpleVec2(0, 0);
	};
	virtual SimpleVec2 GetImGuiImageUV1() override {
		return SimpleVec2(1, 1);
	};

	virtual GraphicsAPITypes GetGraphicsType() override {return GraphicsAPITypes::DX11;};

protected:
	virtual void InitImGui_Internal() override;

private:
	bool InitGfx() override;

	struct IDXGISwapChain* m_SwapChain;//backbuffer
	struct ID3D11Device* m_Device;//dx11 non rendering stuff
	struct ID3D11DeviceContext* m_DevCon;//dx11 rendering stuff

	//Might move these to a Render Target that is the main buffer
	struct ID3D11RenderTargetView* m_RenderTargetView;//main buffer
	struct ID3D11DepthStencilView* m_DepthStencilView;
	struct ID3D11Texture2D* m_DepthStencilBuffer;

	struct ID3D11RasterizerState* m_MainRasterState;
	struct ID3DUserDefinedAnnotation* m_PerfDebug;
	struct ID3D11Debug* m_DebugContext;
};
