#pragma once

#include "SimpleMath.h"
#include "Types/CMString.h"
#include "FwdDec.h"
#include "GFXConstants.h"

class Window;

enum GraphicsAPITypes {
	OPENGL4,
	DX11
};

enum CullState {
	Back,
	Front,
};

class GFX {
public:
	virtual bool CreateWindowSetUpAPI() = 0;
	virtual void DestroyMainWindow() = 0;

	void ImGuiInit();

	void SetUpGraphics();

	virtual void ImGuiDraw() = 0;
	virtual void ImGuiNewFrame() = 0;

	virtual void SwapBuffer() = 0;
	virtual void Clear() = 0;
	virtual void SetClearColor(float R, float G, float B, float A) {
		m_ClearR = R;
		m_ClearG = G;
		m_ClearB = B;
		m_ClearA = A;
	};

	virtual void ResizeWindow(int a_Width, int a_Height);
	virtual void SetFpsLimit(int a_NewLimit) = 0;
	virtual int GetFpsLimit() { return m_FpsLimit; };

	virtual Shader* CreateShader() = 0;
	virtual Mesh* CreateMesh() = 0;
	virtual Texture* CreateTexture() = 0;
	virtual RenderTarget* CreateRenderTarget(int a_Width, int a_Height) = 0;

	virtual ShaderUniformBlock* CreateBuffer(void* a_Object, uint a_Size) = 0;

	virtual void BindTexture(Texture* a_Tex, uint a_Slot) = 0;
	virtual void UnbindTexture(uint a_Slot) = 0;
	virtual void BindTextures(Texture* a_Tex[MAX_TEXTURE_NUM]) = 0;

	virtual void UseRenderTarget(RenderTarget* a_Rt, SimpleBox a_ViewPort);
	virtual void UseRenderTarget(RenderTarget* a_Rt);
	virtual void SetViewPort(SimpleBox a_ViewPort) = 0;

	virtual void ResetRenderTarget() = 0;
	virtual void ResetShader() = 0;

	virtual void PushDebugGroup(CMString a_Name) = 0;
	virtual void PopDebugGroup() = 0;

	virtual SimpleVec2 GetImGuiImageUV0() = 0;
	virtual SimpleVec2 GetImGuiImageUV1() = 0;

	virtual GraphicsAPITypes GetGraphicsType() = 0;

	virtual void SetCullState(CullState a_State) = 0;

	Window* m_Window;

	Texture* m_WhiteTexture;
protected:
	virtual bool InitGfx() = 0;
	virtual void InitImGui_Internal() = 0;
	virtual void ResizeWindow_Internal(int a_Width, int a_Height) = 0;


	float m_ClearR = 0, m_ClearG = 0, m_ClearB = 0, m_ClearA = 1;

	int m_FpsLimit = 60;

	//reference to hold which textures are in each slot
	Texture* m_TextureSlots[MAX_TEXTURE_NUM] = { nullptr };
};

extern class GFX* _CGraphics;
