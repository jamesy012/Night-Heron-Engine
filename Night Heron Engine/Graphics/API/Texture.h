#pragma once

#include <string>

class Texture {
public:
	void loadTexture(std::string a_Path);
	virtual void bind(unsigned int a_Slot) = 0;

protected:
	virtual void createData() = 0;

	unsigned char* m_TextureData;
	int m_Width;
	int m_Height;
	int m_ImageFormat;
	std::string m_Path;
};