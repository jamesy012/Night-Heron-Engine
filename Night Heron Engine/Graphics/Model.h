#pragma once

#include "Types.h"
#include "API/GFXObj.h"

class Model : public GFXObj {
public:
	Model();
	~Model();

	void LoadModel(CMString a_FileName);

	void Draw();

private:
	virtual void SetDebugObjName_Internal() override;

	void ProcessNode(struct aiNode* node, const struct aiScene* scene);
	class Mesh* ProcessMesh(struct aiMesh* mesh, const struct aiScene* scene);

	struct ModelMeshHolder {
		class Mesh* m_Mesh;
		CMString m_ObjName;
	};

	CMArray<ModelMeshHolder> m_Meshs;
};

