#include "Mesh.h"

void Mesh::CreateTriangle() {
	//Vertex v[] = {
	//	{ glm::vec3(0.0f, 0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{ glm::vec3(0.5f, -0.5, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
	//	{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) },
	//};
	//
	//m_Vertices.push_back(v[0]);
	//m_Vertices.push_back(v[1]);
	//m_Vertices.push_back(v[2]);
	//
	//m_Indices.push_back(0);
	//m_Indices.push_back(1);
	//m_Indices.push_back(2);


	//const Vertex OurVertices[] = {
	//	{ { 0.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	//{ { 0.45f, -0.5, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	//{ { -0.45f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	//{ { 0.45f, -0.5, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	//{ { 0.0f, -0.8f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	//{ { -0.45f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	//};

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

	m_Vertices.reserve(indexSize);
	m_Indices.reserve(indexSize);
	for (unsigned int i = 0; i < indexSize; i += 3) {
		Vertex vert;
		vert.m_Pos = vertPos[indexData[i] - 1];
		vert.m_Color = glm::vec4(1, 1, 1, 1);
		vert.m_UV = texCoords[indexData[i + 1] - 1];
		//vert.normal = normals[indexData[i + 2] - 1];

		m_Vertices.push_back(vert);
		m_Indices.push_back(i / 3);
	}
}
