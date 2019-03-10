#pragma once

#include "..\API\Texture.h"

class TextureGL : public Texture {
public:
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