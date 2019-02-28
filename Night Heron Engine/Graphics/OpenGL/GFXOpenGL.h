#pragma once

#include "..\API\GFXAPI.h"

class GFXOpenGL : public GFX {
public:
	~GFXOpenGL();

	bool CreateWindowSetUpAPI() override;
	virtual void DestroyMainWindow() override;

	virtual void ImGuiDraw() override;
	virtual void ImGuiNewFrame() override;

	void SwapBuffer() override;
	void Clear() override;
	void SetClearColor(float R, float G,float B, float A) override;

	virtual void ResizeWindow_Internal(int a_Width, int a_Height) override;

	virtual class Shader* CreateShader() override;
	virtual class Mesh* CreateMesh() override;
	virtual class Texture* CreateTexture() override;
	virtual class RenderTarget* CreateRenderTarget(int a_Width, int a_Height) override;


	virtual ShaderUniformBlock* CreateBuffer(void * a_Object, unsigned int a_Size) override;

protected:
	virtual void InitImGui_Internal() override;

private:
	bool InitGfx() override;

};