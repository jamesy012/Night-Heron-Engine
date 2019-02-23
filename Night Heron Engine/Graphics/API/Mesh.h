#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <vector>

struct Vertex {
	glm::vec3 m_Pos;
	glm::vec2 m_UV;
	//glm::vec3 m_Normal;
	glm::vec4 m_Color;
};

typedef Vertex MeshVerticesType;
typedef unsigned int MeshIndicesType;

class Mesh {
public:

	std::vector<MeshVerticesType> m_Vertices;
	std::vector<MeshIndicesType> m_Indices;

	virtual void Bind() = 0;

	virtual void Draw() = 0;

	void CreateTriangle();

};