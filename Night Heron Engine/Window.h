#pragma once

#include <Windows.h>

#include <string>

class Window {
public:

	virtual bool CreateMainWindow();
	virtual void DestrowMainWindow();

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);               // Declaration For WndProc

	void ImGuiInit();
	void ImGuiNewFrame();

	float GetAspect() {
		return (float)m_WindowWidth / m_WindowHeight;
	}

	HDC m_HDC = NULL; // Private GDI Device Context
	HWND m_HWnd = NULL; // Holds Our Window Handle
	HINSTANCE m_HInstance; // Holds The Instance Of The Application
	HGLRC m_HRC; // Permanent Rendering Context

	unsigned char m_Bits = 16;
	int m_WindowWidth = 1024;
	int m_WindowHeight = 768;

	std::string m_WindowTitle = "Night Heron Engine";
	std::string m_WindowClassName = "NightHeronEngineWndClass";
};