#include "stdafx.h"
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

#include "nlohmann/json.hpp"

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
		for (uint i = 0; i < LastMaterial->m_TextureOverloads.Length(); i++) {
			_CGraphics->UnbindTexture(LastMaterial->m_TextureOverloads[i].m_Slot);
		}
	}

	LastMaterial = this;

	if (_CCurrentShader != m_Shader) {
		m_Shader->Use();
	}

	for (uint i = 0; i < m_TextureOverloads.Length(); i++) {
		_CGraphics->BindTexture(m_TextureOverloads[i].m_Tex, m_TextureOverloads[i].m_Slot);
	}
}

void Material::AddTexture(Texture* a_Texture, uint a_Slot) {
	for (uint i = 0; i < m_TextureOverloads.Length(); i++) {
		if (m_TextureOverloads[i].m_Slot == a_Slot) {
			CMASSERT_MSG(true, "Texture already in slot");
		}
	}
	m_TextureOverloads.Add({ a_Texture, a_Slot });
}

bool Material::LoadData_Internal(nlohmann::json & a_Json) {
	CMString debugName;
	a_Json["Debug Name"].get_to(debugName);
	SetDebugObjName(debugName);

	if (a_Json.contains("Shader")) {
		m_Shader = _CShaderManager->GetShader(a_Json["Shader"].get<CMString>());
		if (m_Shader == nullptr) {
			return false;
		}
	}
	if (a_Json.contains("Textures")) {
		auto textures = a_Json["Textures"];
		for (nlohmann::json::iterator it = textures.begin(); it != textures.end(); ++it) {
			auto tex = *it;
			Texture* texture = _CTextureManager->GetTexture(tex["Path"].get<CMString>());
			if (texture == nullptr) {
				return false;
			}
			AddTexture(texture, tex["Slot"].get<int>());
		}
	}

	return true;
}

void Material::SaveData_Internal(nlohmann::json & a_Json) {
	a_Json["Debug Name"] = m_DebugName;
	if (m_Shader) {
		if (!m_Shader->m_FilePath.m_FilePath.IsEmpty()) {
			a_Json["Shader"] = m_Shader->m_FilePath.m_FilePath;
		} else {
			auto shaders = a_Json["Shaders"];
			for (uint i = 0; i < m_Shader->m_ShaderFileObjects.Length(); i++) {
				shaders[i] = m_Shader->m_ShaderFileObjects[i]->m_FilePath.m_FilePath;
			}
		}
	}
	if (m_TextureOverloads.Length() != 0) {
		auto& textures = a_Json["Textures"];
		for (uint i = 0; i < m_TextureOverloads.Length(); i++) {
			auto& tex = textures[i];
			tex["Slot"] = m_TextureOverloads[i].m_Slot;
			tex["Path"] = m_TextureOverloads[i].m_Tex->GetPath();
		}
	}
}
