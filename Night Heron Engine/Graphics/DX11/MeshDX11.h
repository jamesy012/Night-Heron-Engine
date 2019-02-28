#pragma once

#include "..\API\Mesh.h"


class MeshDX11 : public Mesh {
public:
	~MeshDX11();

	// Inherited via Mesh
	virtual void Bind() override;

	virtual void Draw() override;

private:
	struct ID3D11Buffer* squareVertBuffer;
	struct ID3D11Buffer* squareIndexBuffer;

};
