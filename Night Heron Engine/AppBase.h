#pragma once

class AppBase {
public:

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void ImguiDraw() = 0;
	virtual void Draw() = 0;
	virtual void Finish() = 0;

};