#pragma once

#include "Types.h"
#include "API/GFXObj.h"
#include "Interfaces/Saveable.h"

class Shader;
class Texture;
class Manager;

class Material : public GFXObj, public Saveable {
	friend Manager;
public:
	Material(CMString a_FilePath);
	~Material();

	void Use();


	Shader* m_Shader;

	uint m_ID;

	void AddTexture(Texture* a_Texture, uint a_Slot);

	//temp while we cant load combined shaders from file
	bool m_CreatedShader = false;
protected:
	bool Load_Internal(CMArray<CMString> a_Splits) override;
	CMString GetData_Internal() override;

	struct TextureSlot {
		Texture* m_Tex;
		uint m_Slot;
	};

	CMArray<TextureSlot> m_TextureOverloads;
};