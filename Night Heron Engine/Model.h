#pragma once

#include "CommonTypes.h"

class Model {
public:
	Model();
	~Model();

	void LoadModel(CMString a_FileName);

	void Draw();

private:
	void ProcessNode(struct aiNode* node, const struct aiScene* scene);
	class Mesh* ProcessMesh(struct aiMesh* mesh, const struct aiScene* scene);

	CMArray<class Mesh*> m_Meshs;
};

