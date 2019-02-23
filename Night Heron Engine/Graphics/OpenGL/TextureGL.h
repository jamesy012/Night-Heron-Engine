#pragma once

#include "..\API\Texture.h"

class TextureGL : public Texture {
public:
	virtual void bind(unsigned int a_Slot) override;
private:
	virtual void createData() override;

	unsigned int m_BufferID;
};