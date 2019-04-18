#include "Singletons.h"

#include "Managers/Manager.h"
#include "Managers/ShaderManager.h"
#include "Managers/ShaderSpirvManager.h"
#include "Managers/TextureManager.h"
#include "Managers/Arguments.h"
#include "Managers/TimeManager.h"

extern Manager* _CManager = nullptr;
extern GFX* _CGraphics = nullptr;
extern ShaderManager* _CShaderManager = nullptr;
extern ShaderSpirvManager* _CShaderSpirvManager = nullptr;
extern TextureManager* _CTextureManager = nullptr;
extern Window* _CMainWindow = nullptr;
extern Arguments* _CArguments = nullptr;
extern TimeManager* _CTimeManager = nullptr;

extern void SingletonManager::CreateSingletons() {
	_CManager = new Manager();
	_CShaderSpirvManager = new ShaderSpirvManager();
	_CShaderSpirvManager->FindAllInFiles();
	_CShaderManager = new ShaderManager();
	_CShaderManager->FindAllInFiles();
	_CTextureManager = new TextureManager();
	_CTextureManager->FindAllInFiles();

	_CTimeManager = new TimeManager();
}
