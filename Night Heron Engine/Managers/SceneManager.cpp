#include "stdafx.h"
#include "SceneManager.h"

#include "Util.h"

#include "Scene.h"

void SceneManager::ImGuiData() {

}

bool SceneManager::IsFileAOurType(CMString a_FilePath) {
	uchar fileHash[HASH_LENGTH] = { 0 };
	a_FilePath.SubStrFindFromEnd('.').ToLower().Hash(fileHash);

	for (uint i = 0; i < m_FileTypes.Length(); i++) {
		if (memcmp(m_FileTypes[i].m_ExtenstionHash, fileHash, HASH_LENGTH) == 0) {
			return true;
		}
	}

	return false;
}

void SceneManager::AddFromPath_Internal(CMString a_FilePath) {
	if (a_FilePath[0] == '\\' || a_FilePath[0] == '/') {
		a_FilePath = a_FilePath.SubStr(1, a_FilePath.Length());
	}
	Scene* scene = new Scene();
	scene->m_FilePath = a_FilePath;
	m_Scenes.Add(scene);
}
