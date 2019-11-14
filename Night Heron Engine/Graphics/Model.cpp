#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SimpleMath.h"

#include "Singletons.h"
#include "Managers/Manager.h"

#include <Graphics/API/GFXAPI.h>
#include <Graphics/API/Mesh.h>

#include "Graphics/Material.h"

Model::Model() {
	_CManager->m_Models.Add(this);
}

Model::~Model() {
	for (uint i = 0; i < m_Meshs.Length(); i++) {
		delete m_Meshs[i].m_Mesh;
	}
	_CManager->m_Models.Remove(this);
}

void Model::LoadModel(CMString a_FileName) {
	Assimp::Importer importer;
	const struct aiScene* scene;

	scene = importer.ReadFile(a_FileName.c_str(), aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

	ProcessNode(scene->mRootNode, scene);

	uint index = a_FileName.FindFromEnd('/') + 1;
	SetDebugObjName(a_FileName.SubStr(index, a_FileName.Size() - index));

	m_FilePath = a_FileName;
}

void Model::CreateSquare() {
	glm::vec4 vertPos[] = {
		glm::vec4(1, -1, -1, 1),
		glm::vec4(1, -1, 1, 1),
		glm::vec4(-1, -1, 1, 1),
		glm::vec4(-1, -1, -1, 1),
		glm::vec4(1, 1, -1, 1),
		glm::vec4(1, 1, 1, 1),
		glm::vec4(-1, 1, 1, 1),
		glm::vec4(-1, 1, -1, 1),
	};
	glm::vec4 normals[]{
		glm::vec4(0, -1, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(1, -0, 0, 0),
		glm::vec4(-0, -0, 1, 0),
		glm::vec4(-1, -0, -0, 0),
		glm::vec4(0, 0, -1, 0),
	};
	glm::vec2 texCoords[]{//y coords have been flipped
		glm::vec2(1, 0),
		glm::vec2(0, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 1),
	};
	unsigned int indexData[]{
		2, 1, 1, 3, 2, 1, 4, 3, 1,
		8, 2, 2, 7, 3, 2, 6, 4, 2,
		1, 4, 3, 5, 1, 3, 6, 2, 3,
		2, 4, 4, 6, 1, 4, 7, 2, 4,
		7, 1, 5, 8, 2, 5, 4, 3, 5,
		1, 3, 6, 4, 4, 6, 8, 1, 6,
		1, 4, 1, 2, 1, 1, 4, 3, 1,
		5, 1, 2, 8, 2, 2, 6, 4, 2,
		2, 3, 3, 1, 4, 3, 6, 2, 3,
		3, 3, 4, 2, 4, 4, 7, 2, 4,
		3, 4, 5, 7, 1, 5, 4, 3, 5,
		5, 2, 6, 1, 3, 6, 8, 1, 6,
	};
	unsigned int indexSize = sizeof(indexData) / sizeof(unsigned int);

	Mesh* mesh = _CGraphics->CreateMesh();

	mesh->m_Vertices.Reserve(indexSize);
	mesh->m_Indices.Reserve(indexSize);
	for (unsigned int i = 0; i < indexSize; i += 3) {
		Vertex vert;
		vert.m_Pos = vertPos[indexData[i] - 1];
		vert.m_Color = glm::vec4(1, 1, 1, 1);
		vert.m_UV = texCoords[indexData[i + 1] - 1];
		vert.m_Normal = normals[indexData[i + 2] - 1];

		mesh->m_Vertices.Add(vert);
		mesh->m_Indices.Add(i / 3);
	}
	mesh->Bind();
	m_Meshs.Add({ mesh, "Model_Square" });
	SetDebugObjName("Square");
	m_FilePath = "GENERATED/Square";
}

void Model::CreatePlane() {
	const glm::vec4 vertPos[] = {
		glm::vec4(-1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(-1, 0, -1, 1),
		glm::vec4(1, 0, -1, 1),
	};
	const glm::vec4 normals[]{
		glm::vec4(0, 1, 0, 0),
	};
	const glm::vec2 texCoords[]{
		glm::vec2(0, 1),
		glm::vec2(1, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
	};
	const unsigned int indexData[]{
		1, 1, 1, 2, 2, 1, 3, 3, 1,
		2, 2, 1, 4, 4, 1, 3, 3, 1,
	};
	const unsigned int indexSize = sizeof(indexData) / sizeof(unsigned int);

	Mesh* mesh = _CGraphics->CreateMesh();

	mesh->m_Vertices.Reserve(indexSize);
	mesh->m_Indices.Reserve(indexSize);
	for (unsigned int i = 0; i < indexSize; i += 3) {
		Vertex vert;
		vert.m_Pos = vertPos[indexData[i] - 1];
		vert.m_Color = glm::vec4(1, 1, 1, 1);
		vert.m_UV = texCoords[indexData[i + 1] - 1];
		vert.m_Normal = normals[indexData[i + 2] - 1];

		mesh->m_Vertices.Add(vert);
		mesh->m_Indices.Add(i / 3);
	}
	mesh->Bind();
	m_Meshs.Add({ mesh, "Model_Plane" });
	SetDebugObjName("Plane");
	m_FilePath = "GENERATED/Plane";
}

void Model::Draw() {
	for (uint i = 0; i < m_Meshs.Length(); i++) {
		if (m_Meshs[i].m_Material) {
			m_Meshs[i].m_Material->Use();
		}
		m_Meshs[i].m_Mesh->Draw();
	}
}

void Model::SetMaterial(Material * a_NewMaterial, uint a_Slot) {
	m_Meshs[a_Slot].m_Material = a_NewMaterial;
}

void Model::SetDebugObjName_Internal() {
	for (uint i = 0; i < m_Meshs.Length(); i++) {
		CMString index = CMString::IntToString(i);
		CMString objName = m_DebugName + " - " + index + " - " + m_Meshs[i].m_ObjName;
		m_Meshs[i].m_Mesh->SetDebugObjName(objName);
	}
}

void Model::ProcessNode(aiNode * node, const aiScene * scene) {
	for (uint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* assimpMesh = scene->mMeshes[node->mMeshes[i]];

		Mesh* mesh = ProcessMesh(assimpMesh, scene);

		CMString objName = assimpMesh->mName.C_Str();
		m_Meshs.Add({ mesh, objName });
	}

	for (uint i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh* Model::ProcessMesh(aiMesh * mesh, const aiScene * scene) {

	CMArray<MeshVerticesType> verties;
	CMArray<MeshIndicesType> indices;

	glm::vec4 matColor;
	bool hasMaterialColor = false;

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D col;
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, col);
		matColor.x = col.r;
		matColor.y = col.g;
		matColor.z = col.b;
		matColor.w = col.a;
		hasMaterialColor = true;
	}

	for (uint i = 0; i < mesh->mNumVertices; i++) {
		MeshVerticesType vert;
		vert.m_Pos.x = mesh->mVertices[i].x;
		vert.m_Pos.y = mesh->mVertices[i].y;
		vert.m_Pos.z = mesh->mVertices[i].z;
		vert.m_Pos.w = 1;

		if (mesh->HasNormals()) {
			vert.m_Normal.x = mesh->mNormals[i].x;
			vert.m_Normal.y = mesh->mNormals[i].y;
			vert.m_Normal.z = mesh->mNormals[i].z;
			vert.m_Normal.w = 1;
		}

		if (mesh->mTextureCoords[0]) {
			vert.m_UV.x = mesh->mTextureCoords[0][i].x;
			vert.m_UV.y = mesh->mTextureCoords[0][i].y;
		} else {
			vert.m_UV = glm::vec2(0, 0);
		}

		if (mesh->HasTangentsAndBitangents()) {
			vert.m_Tangent.x = mesh->mTangents[i].x;
			vert.m_Tangent.y = mesh->mTangents[i].y;
			vert.m_Tangent.z = mesh->mTangents[i].z;
			vert.m_BiTangent.x = mesh->mBitangents[i].x;
			vert.m_BiTangent.y = mesh->mBitangents[i].y;
			vert.m_BiTangent.z = mesh->mBitangents[i].z;
		} else {
			vert.m_Tangent = glm::vec3(0, 1, 0);
			vert.m_BiTangent = glm::vec3(0, 1, 0);
		}

		if (mesh->mColors[0]) {
			vert.m_Color.x = mesh->mColors[0][i].r;
			vert.m_Color.y = mesh->mColors[0][i].g;
			vert.m_Color.z = mesh->mColors[0][i].b;
			vert.m_Color.w = mesh->mColors[0][i].a;
		} else {
			if (hasMaterialColor) {
				vert.m_Color = matColor;
			} else {
				vert.m_Color = glm::vec4(1, 0, 1, 1);
			}
		}

		verties.Add(vert);
	}

	for (uint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (uint j = 0; j < face.mNumIndices; j++) {
			indices.Add(face.mIndices[j]);
		}
	}

	Mesh* newMesh = _CGraphics->CreateMesh();
	newMesh->m_Vertices = verties;
	newMesh->m_Indices = indices;
	newMesh->Bind();

	return newMesh;
}
