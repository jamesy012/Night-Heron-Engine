#pragma once

#include "Types.h"
#include "GFXObj.h"

class GFX;

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

	unsigned char* m_TextureData;
	int m_Width;
	int m_Height;
	int m_ImageFormat;
	CMString m_Path;

};