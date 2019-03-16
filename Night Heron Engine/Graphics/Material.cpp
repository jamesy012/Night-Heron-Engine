#include "Material.h"

#include "Graphics/API/Shader.h"
#include "Graphics/API/Texture.h"

#include "Singletons.h"
#include "Manager.h"

Material::Material() {
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
