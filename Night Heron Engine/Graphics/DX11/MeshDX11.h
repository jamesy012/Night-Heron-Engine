#pragma once

#include "..\API\Mesh.h"


class MeshDX11 : public Mesh {
public:
	~MeshDX11();

	// Inherited via Mesh
	virtual void Bind() override;

	virtual void Draw() override;

private:
	virtual void SetDebugObjName_Internal() override;

	struct ID3D11Buffer* m_VertexBuffer;
	struct ID3D11Buffer* m_IndexBuffer;

};
