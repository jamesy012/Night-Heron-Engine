#pragma once

#include "SimpleMath.h"
#include "Types.h"
#include "FwdDec.h"

class Window;


class GFX {
public:
	virtual bool CreateWindowSetUpAPI() = 0;
	virtual void DestroyMainWindow() = 0;

	void ImGuiInit();
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

	virtual Shader* CreateShader() = 0;
	virtual Mesh* CreateMesh() = 0;
	virtual Texture* CreateTexture() = 0;
	virtual RenderTarget* CreateRenderTarget(int a_Width, int a_Height) = 0;

	virtual ShaderUniformBlock* CreateBuffer(void* a_Object, uint a_Size) = 0;

	virtual void BindTexture(Texture* a_Tex, uint a_Slot) = 0;
	virtual void UnbindTexture(uint a_Slot) = 0;

	virtual void UseRenderTarget(RenderTarget* a_Rt) = 0;
	virtual void ResetRenderTarget() = 0;
	virtual void ResetShader() = 0;

	virtual void PushDebugGroup(CMString a_Name) = 0;
	virtual void PopDebugGroup() = 0;

	virtual SimpleVec2 GetImGuiImageUV0() = 0;
	virtual SimpleVec2 GetImGuiImageUV1() = 0;

	Window* m_Window;
protected:
	virtual bool InitGfx() = 0;
	virtual void InitImGui_Internal() = 0;
	virtual void ResizeWindow_Internal(int a_Width, int a_Height) = 0;

	float m_ClearR = 0, m_ClearG = 0, m_ClearB = 0, m_ClearA = 1;

#define NUM_OF_TEXTURE_SLOTS 32
	//reference to hold which textures are in each slot
	Texture* m_TextureSlots[NUM_OF_TEXTURE_SLOTS] = {nullptr};

};
