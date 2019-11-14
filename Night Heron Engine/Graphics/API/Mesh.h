#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <vector>

#include "Types/CMArray.h"

#include "GFXObj.h"

struct Vertex {
	glm::vec4 m_Pos;
	glm::vec4 m_Normal;
	glm::vec2 m_UV;
	glm::vec4 m_Color;
	glm::vec3 m_Tangent;
	glm::vec3 m_BiTangent;
};

typedef Vertex MeshVerticesType;
typedef unsigned int MeshIndicesType;

class Mesh : public GFXObj {
public:

	CMArray<MeshVerticesType> m_Vertices;
	CMArray<MeshIndicesType> m_Indices;

	virtual void Bind() = 0;

	virtual void Draw() = 0;

	void CreateSquare();
};
