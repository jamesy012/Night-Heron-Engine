#pragma once

#include "SimpleMath.h"
#include "Types/CMString.h"

#include "InputKeys.h"

class Window;

extern class InputHandler* _CInput;

class InputHandler {
public:
	InputHandler();
	~InputHandler();

	bool Startup(Window* a_InputWindow);
	void Shutdown();

	void Update();

	CMString GetKeysDown();

	bool IsKeyDown(IKeys a_Key);
	bool WasKeyPressed(IKeys a_Key);
	SimpleVec2 GetMousePos();
	SimpleVec2 GetMouseDelta();
	bool IsMouseKeyDown(IMouseKeys a_Key);
	bool WasMouseKeyPressed(IMouseKeys a_Key);

private:

	bool ReadKeyboard();
	bool ReadMouse();

	Window* m_MainWindow;

};

