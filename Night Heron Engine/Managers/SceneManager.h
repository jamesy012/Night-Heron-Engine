#pragma once

#include "ManagerBase.h"

#include "Types.h"
class Scene;
class Manager;

extern class SceneManager* _CSceneManager;

class SceneManager : public ManagerBase {
	friend Manager;
public:
	//SceneManager();
	//~SceneManager();

	void ImGuiData();

private:
	bool IsFileAOurType(CMString a_FilePath) override;
	void AddFromPath_Internal(CMString a_FilePath) override;

	const CMArray<CMStringHash> m_FileTypes = { ".scene" };

	CMArray<Scene*> m_Scenes;
};
