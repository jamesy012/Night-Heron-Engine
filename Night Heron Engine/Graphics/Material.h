#pragma once

#include "Types.h"
#include "API/GFXObj.h"

class Shader;
class Texture;

class Material : public GFXObj {
public:
	Material();
	~Material();

	void Use();

	Shader* m_Shader;

	uint m_ID;

	struct TextureSlot {
		Texture* m_Tex;
		uint m_Slot;
	};

	//CMArray<TextureSlot> m_TextureOverloads;
	CMArray<Texture*> m_TextureOverloads;
};