#pragma once

#include <string>
#include "GFXObj.h"

class Texture : public GFXObj {
public:
	void LoadTexture(std::string a_Path);
	void CreateTexture(int a_Width, int a_Height);

	virtual void* getTexturePtr() = 0;

protected:
	virtual void createData() = 0;

	unsigned char* m_TextureData;
	int m_Width;
	int m_Height;
	int m_ImageFormat;
	std::string m_Path;

};