#pragma once

#include "..\API\Texture.h"

class TextureGL : public Texture {
public:
	virtual void bind(unsigned int a_Slot) override;

	void* getTexturePtr() override {
		return (void*)m_BufferID;
	}

	unsigned int getBufferID() {
		return m_BufferID;
	}
private:
	virtual void createData() override;

	unsigned int m_BufferID;
};