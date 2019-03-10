#pragma once

#include "..\API\Mesh.h"

class MeshGL : public Mesh {
	~MeshGL();

	// Inherited via Mesh
	virtual void Bind() override;

	virtual void Draw() override;

private:
	virtual void SetDebugObjName_Internal() override;

	unsigned int m_Vao, m_Vbo, m_Ebo;

};