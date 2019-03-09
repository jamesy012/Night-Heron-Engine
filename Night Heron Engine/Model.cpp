#include "Model.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SimpleMath.h"

#include <Graphics/API/GFXAPI.h>
#include <Graphics/API/Mesh.h>

Model::Model() {
}


Model::~Model() {
	for (int i = 0; i < m_Meshs.size();i++) {
		delete m_Meshs[i];
	}
}

void Model::LoadModel(CMString a_FileName) {
	Assimp::Importer importer;
	const struct aiScene* scene;

	scene = importer.ReadFile(a_FileName.c_str(), aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	ProcessNode(scene->mRootNode, scene);

}

void Model::Draw() {
	for (int i = 0; i < m_Meshs.size(); i++) {
		m_Meshs[i]->Draw();
	}
}

void Model::ProcessNode(aiNode * node, const aiScene * scene) {
	for (uint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshs.push_back(ProcessMesh(mesh, scene));
	}

	for (uint i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh * Model::ProcessMesh(aiMesh * mesh, const aiScene * scene) {
	/*
	// Data to fill
	vector<VERTEX> vertices;
	vector<UINT> indices;
	vector<Texture> textures;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

		if (textype.empty()) textype = determineTextureType(scene, mat);
	}

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.X = mesh->mVertices[i].x;
		vertex.Y = mesh->mVertices[i].y;
		vertex.Z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	return Mesh(dev, vertices, indices, textures);
	*/

	CMArray<MeshVerticesType> verties;
	CMArray<MeshIndicesType> indices;

	glm::vec4 matColor;
	bool hasMaterialColor = false;

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D col;
		mat->Get(AI_MATKEY_COLOR_DIFFUSE,col);
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

		if (mesh->mTextureCoords[0]) {
			vert.m_UV.x = mesh->mTextureCoords[0][i].x;
			vert.m_UV.y = mesh->mTextureCoords[0][i].y;
		} else {
			vert.m_UV = glm::vec2(0, 0);
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

		verties.push_back(vert);
	}

	for (uint i = 0; i < mesh->mNumFaces; i++) {

		aiFace face = mesh->mFaces[i];

		for (uint j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}


	Mesh* newMesh = m_CurrentGraphics->CreateMesh();
	newMesh->m_Vertices = verties;
	newMesh->m_Indices = indices;
	newMesh->Bind();

	return newMesh;
}
