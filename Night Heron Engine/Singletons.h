#pragma once

extern class Manager* _CManager;
extern class GFX* _CGraphics;
extern class ShaderManager* _CShaderManager;
extern class ShaderSpirvManager* _CShaderSpirvManager;
extern class TextureManager* _CTextureManager;
extern class Window* _CMainWindow;
extern class Arguments* _CArguments;
extern class TimeManager* _CTimeManager;
extern class IniFile* _CIniFileManager;
extern class InputHandler* _CInput;

namespace SingletonManager {
	extern void CreateSingletons();
};
