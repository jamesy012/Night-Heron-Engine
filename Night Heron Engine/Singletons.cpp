#include "Singletons.h"

#include "Manager.h"
#include "Graphics/ShaderManager.h"


extern Manager* _CManager = nullptr;
extern GFX* _CGraphics = nullptr;
extern ShaderManager* _CShaderManager = nullptr;


extern void SingletonManager::CreateSingletons() {
	_CManager = new Manager();
	_CShaderManager = new ShaderManager();
	_CShaderManager->FindAllShaders();
}
