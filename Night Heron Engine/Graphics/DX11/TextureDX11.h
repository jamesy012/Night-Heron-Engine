#pragma once

#include "..\API\Texture.h"

class TextureDX11 : public Texture {
public:
	virtual void Bind(unsigned int a_Slot) override;
	virtual void UnBind() override;

	void* getTexturePtr() override {
		return m_TextureSRV;
	}

	struct ID3D11Texture2D* Get2DTextrueRef() {
		return m_TextureRef;
	}

	unsigned int m_ExtraBindFlags = 0;
	unsigned int m_DesiredFormat = 0;
	bool m_IsShaderResource = true;
	//Might change how this works.. not every texture needs it's own sampler
	bool m_ShouldCreateSampler = true;
private:
	virtual void createData() override;

	struct ID3D11Texture2D* m_TextureRef;
	struct ID3D11ShaderResourceView* m_TextureSRV;
	struct ID3D11SamplerState* m_SamplerRef;

};