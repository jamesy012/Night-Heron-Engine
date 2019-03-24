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
			}
		}
		if (stage == 3) {
			int shaders = CMString::StringToInt(a_Splits[line++]);
			if (m_Shader == nullptr) {
				m_Shader = _CGraphics->CreateShader();
				m_Shader->SetDebugObjName("Material " + GetDebugObjName() + " Temp Shader");
				for (int i = 0; i < shaders; i++) {
					m_Shader->AddShader(_CShaderManager->GetShaderPart(a_Splits[line++]));
				}
				m_Shader->LinkShaders();
				m_CreatedShader = true;
			} else {
				line += shaders;
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
	return data;
}
