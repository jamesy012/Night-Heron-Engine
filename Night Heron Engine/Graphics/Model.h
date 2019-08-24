#pragma once

#include "Types/CMString.h"
#include "Types/CMArray.h"
#include "API/GFXObj.h"

class Material;

class Model : public GFXObj {
public:

	struct ModelMeshHolder {
		class Mesh* m_Mesh;
		CMString m_ObjName;
		Material* m_Material;
	};

public:
	Model();
	~Model();

	void LoadModel(CMString a_FileName);
	void CreateSquare();
	void CreatePlane();

	void Draw();

	void SetMaterial(Material* a_NewMaterial, uint a_Slot);

	uint NumMeshs() {
		return m_Meshs.Length();
	}

	ModelMeshHolder* GetMeshHolder(uint a_Index) {
		return &m_Meshs[a_Index];
	}

	CMString m_FilePath;
private:
	virtual void SetDebugObjName_Internal() override;

	void ProcessNode(struct aiNode* node, const struct aiScene* scene);
	class Mesh* ProcessMesh(struct aiMesh* mesh, const struct aiScene* scene);

	CMArray<ModelMeshHolder> m_Meshs;
};
