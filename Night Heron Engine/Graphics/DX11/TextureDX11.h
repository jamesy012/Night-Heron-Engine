#pragma once

#include "..\API\Texture.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

class TextureDX11 : public Texture {
public:

	void* getTexturePtr() override {
		return m_TextureSRV;
	}

	ID3D11Texture2D* Get2DTextrueRef() {
		return m_TextureRef;
	}

	ID3D11ShaderResourceView* GetTextureSRV() const {
		return m_TextureSRV;
	};
	ID3D11SamplerState* GetSamplerRef() const {
		return m_SamplerRef;
	};
	void SetSamplerRef(ID3D11SamplerState* a_NewRef) {
		m_SamplerRef = a_NewRef;
	};

	virtual void SetDebugObjName_Internal() override;

	unsigned int m_ExtraBindFlags = 0;
	unsigned int m_DesiredFormat = 0;
	unsigned int m_DesiredShaderResourceFormat = 0;
	bool m_IsShaderResource = true;
	//Might change how this works.. not every texture needs it's own sampler
	bool m_ShouldCreateSampler = true;
private:
	virtual void createData() override;

	ID3D11Texture2D* m_TextureRef;
	ID3D11ShaderResourceView* m_TextureSRV;
	ID3D11SamplerState* m_SamplerRef;
};
