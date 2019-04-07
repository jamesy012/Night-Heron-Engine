#pragma once

extern class Manager* _CManager;
extern class GFX* _CGraphics;
extern class ShaderManager* _CShaderManager;
extern class ShaderSpirvManager* _CShaderSpirvManager;
extern class TextureManager* _CTextureManager;
extern class Window* _CMainWindow;
extern class Arguments* _CArguments;

namespace SingletonManager {
	extern void CreateSingletons();
};
