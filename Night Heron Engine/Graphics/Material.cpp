#include "Material.h"

#include "Graphics/API/Shader.h"
#include "Graphics/API/Texture.h"

#include "Singletons.h"

#include "Graphics/API/GFXAPI.h"

#include "Managers/ShaderManager.h"
#include "Managers/ShaderSpirvManager.h"
#include "Managers/TextureManager.h"
#include "Managers/Manager.h"

#include "Debug.h"

static Material* LastMaterial = nullptr;

Material::Material(CMString a_FilePath) : Saveable(a_FilePath) {
	_CManager->m_Materials.Add(this);
	static uint counter = 0;
	m_ID = counter++;
}

Material::~Material() {
	_CManager->m_Materials.Remove(this);
}

void Material::Use() {
	if (LastMaterial != nullptr) {
		for (int i = 0; i < LastMaterial->m_TextureOverloads.Length(); i++) {
			_CGraphics->UnbindTexture(LastMaterial->m_TextureOverloads[i].m_Slot);
		}
	}

	LastMaterial = this;

	if (_CCurrentShader != m_Shader) {
		m_Shader->Use();
	}

	for (int i = 0; i < m_TextureOverloads.Length(); i++) {
		_CGraphics->BindTexture(m_TextureOverloads[i].m_Tex, m_TextureOverloads[i].m_Slot);
	}
}

void Material::AddTexture(Texture* a_Texture, uint a_Slot) {
	for (int i = 0; i < m_TextureOverloads.Length(); i++) {
		if (m_TextureOverloads[i].m_Slot == a_Slot) {
			CMASSERT_MSG(true, "Texture already in slot");
		}
	}
	m_TextureOverloads.Add({ a_Texture, a_Slot });
}

bool Material::Load_Internal(CMArray<CMString> a_Splits) {
	uint line = 0;
	uint stage = 0;
	while (line < a_Splits.Length() - 1) {
		stage++;
		if (stage == 1) {
			SetDebugObjName(a_Splits[line++]);
		}
		if (stage == 2) {
			CMString path = a_Splits[line++];
			if (!path.IsEmpty()) {
				m_Shader = _CShaderManager->GetShader(path);
				if (m_Shader == nullptr) {
					return false;
				}
			}
		}
		if (stage == 3) {
			int shaders = CMString::StringToInt(a_Splits[line++]);
			if (m_Shader == nullptr) {
				m_Shader = _CGraphics->CreateShader();
				m_Shader->SetDebugObjName("Material " + GetDebugObjName() + " Temp Shader");
				for (int i = 0; i < shaders; i++) {
					m_Shader->AddShader(_CShaderSpirvManager->GetShaderPart(a_Splits[line++]));
				}
				m_Shader->LinkShaders();
				m_CreatedShader = true;
			} else {
				line += shaders;
			}
		}
		if (stage == 4) {
			int textures = CMString::StringToInt(a_Splits[line++]);
			for (int i = 0; i < textures; i++) {
				int slot = CMString::StringToInt(a_Splits[line++]);;
				CMString path = a_Splits[line++];
				Texture* tex = _CTextureManager->GetTexture(path);
				if (tex == nullptr) {
					return false;
				}
				AddTexture(tex, slot);
			}
		}
	}

	return true;
}

CMString Material::GetData_Internal() {
	CMString data;
	data += m_DebugName + "\n";
	if (m_Shader) {
		if (!m_Shader->m_FilePath.m_FilePath.IsEmpty()) {
			data += m_Shader->m_FilePath.m_FilePath + '\n';
			data += "0\n";
		} else {
			data += "" + '\n';
			data += CMString::IntToString(m_Shader->m_ShaderFileObjects.Length()) + '\n';
			for (uint i = 0; i < m_Shader->m_ShaderFileObjects.Length(); i++) {
				data += m_Shader->m_ShaderFileObjects[i]->m_FilePath.m_FilePath + '\n';
			}
		}
	} else {
		data += "\n0\n";
	}
	{
		data += CMString::IntToString(m_TextureOverloads.Length()) + '\n';
		for (uint i = 0; i < m_TextureOverloads.Length(); i++) {
			data += CMString::IntToString(m_TextureOverloads[i].m_Slot) + '\n';
			data += m_TextureOverloads[i].m_Tex->GetPath() + '\n';
		}
	}

	return data;
}
