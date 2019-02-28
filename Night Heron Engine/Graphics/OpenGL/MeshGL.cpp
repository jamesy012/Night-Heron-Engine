#pragma once

#include "MeshGL.h"

#include <GL/glew.h>

MeshGL::~MeshGL() {
	if (m_Vao) {
		glDeleteVertexArrays(1, &m_Vao);
	}
	if (m_Vbo) {
		glDeleteBuffers(1, &m_Vbo);
	}
	if (m_Ebo) {
		glDeleteBuffers(1, &m_Ebo);
	}
	m_Vao = m_Vbo = m_Ebo = 0;
}

void MeshGL::Bind() {
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);
	//glGenBuffers(2, &m_Vbo); // this should do the same as above?!

	//bind all future data to the vao
	glBindVertexArray(m_Vao);

	//bind data
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(MeshVerticesType), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(MeshIndicesType), &m_Indices[0], GL_STATIC_DRAW);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVerticesType), (GLvoid*)0);

	////normal
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVerticesType), (GLvoid*)offsetof(Vertex, normal));
	//
	//texture coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVerticesType), (GLvoid*)offsetof(Vertex, m_UV));

	//Vertex Colors
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVerticesType), (GLvoid*)offsetof(Vertex, m_Color));

	glBindVertexArray(0);
}

void MeshGL::Draw() {
	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
}
