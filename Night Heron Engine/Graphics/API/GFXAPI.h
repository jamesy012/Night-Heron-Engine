#pragma once

class Window;


class GFX {
public:

public:
	Window* m_Window;
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

	virtual class Shader* CreateShader() = 0;
	virtual class Mesh* CreateMesh() = 0;
	virtual class Texture* CreateTexture() = 0;

	virtual class ShaderUniformBlock* CreateBuffer(void* a_Object, unsigned int a_Size) = 0;


protected:
	virtual bool InitGfx() = 0;
	virtual void InitImGui_Internal() = 0;
	virtual void ResizeWindow_Internal(int a_Width, int a_Height) = 0;

	float m_ClearR = 0, m_ClearG = 0, m_ClearB = 0, m_ClearA = 1;



};

//namespace Constants {
	extern GFX* m_CurrentGraphics;
//}
