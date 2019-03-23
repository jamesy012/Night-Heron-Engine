#include "Material.h"

#include "Graphics/API/Shader.h"
#include "Graphics/API/Texture.h"

#include "Singletons.h"
#include "Manager.h"

#include "Graphics/API/GFXAPI.h"
#include "Graphics/ShaderManager.h"

Material::Material(CMString a_FilePath) : Saveable(a_FilePath) {
	_CManager->m_Materials.Add(this);
	static uint counter = 0;
	m_ID = counter++;
}

Material::~Material() {
	_CManager->m_Materials.Remove(this);
}

void Material::Use() {
	if (_CCurrentShader != m_Shader){
		m_Shader->Use();
	}

	//for (int i = 0; i < m_TextureOverloads.size(); i++) {
	//	m_TextureOverloads[i].
	//}
}

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include <windows.h>

bool Material::Load() {
	CMString data = Util::LoadTextFromPath(m_FilePath.m_FilePath);
	if (data.Length() < 5) {
		return false;
	}
	CMArray<CMString> splits = data.Split('\n');
	int line = 0;
	while (line < splits.Length()) {
		line++;
		if (line == 1) {
			SetDebugObjName(splits[line]);
		}
		if (line == 2) {
			int shaders = CMString::StringToInt(splits[line++]);
			m_Shader = _CGraphics->CreateShader();
			m_Shader->SetDebugObjName("Material " + GetDebugObjName() + " Temp Shader");
			for (int i = 0; i < shaders; i++) {
				m_Shader->AddShader(_CShaderManager->GetShader(splits[line++]));
			}
			m_Shader->LinkShaders();
			m_CreatedShader = true;
		}
	}
	
	return true;
}

void Material::Save() {
	CreateDirectory(m_FilePath.m_FileLocation.Get(), NULL);
	std::ofstream infoFile(m_FilePath.m_FilePath);
	CMString data;
	if (infoFile.is_open()) {

		data += m_DebugName += "\n";
		if (m_Shader) {
			data += CMString::IntToString(m_Shader->m_ShaderFileObjects.Length()) + '\n';
			for (uint i = 0; i < m_Shader->m_ShaderFileObjects.Length(); i++) {
				data += m_Shader->m_ShaderFileObjects[i]->m_FilePath.m_FilePath + '\n';
			}
		} else {
			data += '0';
		}
		data += "\n";

		data.Hash(m_Hash);
		for (int q = 0; q < HASH_LENGTH; q++) {
			infoFile << m_Hash[q];
		}
		infoFile << "\n";
		infoFile << data;
		infoFile.close();
	}
	
}
