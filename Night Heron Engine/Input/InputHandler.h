#pragma once

#include "SimpleMath.h"
#include "Types.h"

#include "InputKeys.h"

class Window;

class InputHandler {
public:
	InputHandler();
	~InputHandler();

	bool Startup(Window* a_InputWindow);
	void Shutdown();

	void Update();

	bool IsKeyDown(IKeys a_Key);
	SimpleVec2 GetMousePos();

	CMString GetKeysDown();

private:

	bool ReadKeyboard();
	bool ReadMouse();

	Window* m_MainWindow;

};

