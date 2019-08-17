#pragma once

#include "Types/CMString.h"
#include "GFXObj.h"

class GFX;

enum TextureSizes {
	None,
	R,
	RG,
	RGB,
	RGBA
};

class Texture : public GFXObj {
	friend GFX;
public:
	void LoadTexture(CMString a_Path);
	void CreateTexture(int a_Width, int a_Height);

	virtual void* getTexturePtr() = 0;

	CMString GetPath() {
		return m_Path;
	}

protected:
	virtual void createData() = 0;

	TextureSizes STBiToSize(int a_Size) const;

	unsigned char* m_TextureData;
	int m_Width;
	int m_Height;
	TextureSizes m_ImageFormat;
	CMString m_Path;
};
