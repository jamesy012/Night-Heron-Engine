#pragma once

#include "..\API\Texture.h"

class TextureDX11 : public Texture {
public:
	virtual void bind(unsigned int a_Slot) override;

	void* getTexturePtr() override {
		return m_TextureSRV;
	}

private:
	virtual void createData() override;

	struct ID3D11Texture2D* m_TextureRef;
	struct ID3D11ShaderResourceView* m_TextureSRV;
	struct ID3D11SamplerState* m_SamplerRef;

};