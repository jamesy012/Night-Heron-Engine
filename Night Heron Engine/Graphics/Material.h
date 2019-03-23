#pragma once

#include "Types.h"
#include "API/GFXObj.h"
#include "Interfaces/Saveable.h"

class Shader;
class Texture;

class Material : public GFXObj, public Saveable {
public:
	Material(CMString a_FilePath);
	~Material();

	void Use();

	bool Load() override;
	void Save() override;

	Shader* m_Shader;

	uint m_ID;

	struct TextureSlot {
		Texture* m_Tex;
		uint m_Slot;
	};

	//CMArray<TextureSlot> m_TextureOverloads;
	CMArray<Texture*> m_TextureOverloads;

	//temp while we cant load combined shaders from file
	bool m_CreatedShader = false;
};