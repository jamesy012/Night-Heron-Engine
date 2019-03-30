#pragma once

#include "ManagerBase.h"

#include "Types.h"
class Texture;

extern class TextureManager* _CTextureManager;

class TextureManager : public ManagerBase {
public:
	//TextureManager();
	//~TextureManager();


	void ImGuiWindow(bool* a_Open) override;

	void AddTexture(Texture* a_Texture);
private:
	bool IsFileAOurType(CMString a_FilePath) override;
	void AddFromPath_Internal(CMString a_FilePath) override;

	const CMArray<CMStringHash> m_FileTypes = { ".png", ".jpg" };

	struct TextureRef {
		Texture* m_Texture;
		bool m_PassedThrough;
		CMString m_FilePath;
	};

	CMArray<TextureRef> m_Textures;

};
