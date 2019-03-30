#pragma once

extern class Manager* _CManager;
extern class GFX* _CGraphics;
extern class ShaderManager* _CShaderManager;
extern class TextureManager* _CTextureManager;
extern class Window* _CMainWindow;

namespace SingletonManager {
	extern void CreateSingletons();
};
