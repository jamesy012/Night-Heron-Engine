#pragma once

#include "..\API\GFXAPI.h"

class GFXOpenGL : public GFX {
public:
	GFXOpenGL();
	~GFXOpenGL();

	bool CreateWindowSetUpAPI() override;
	virtual void DestroyMainWindow() override;

	virtual void ImGuiDraw() override;
	virtual void ImGuiNewFrame() override;

	void SwapBuffer() override;
	void Clear() override;
	void SetClearColor(float R, float G, float B, float A) override;

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
		return SimpleVec2(0, 1);
	};
	virtual SimpleVec2 GetImGuiImageUV1() override {
		return SimpleVec2(1, 0);
	};

	virtual GraphicsAPITypes GetGraphicsType() override { return GraphicsAPITypes::OPENGL4; };

	virtual void SetCullState(CullState a_State);

protected:
	virtual void InitImGui_Internal() override;

private:
	bool InitGfx() override;
};
