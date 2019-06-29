#version 450
layout(location = 0) in vec4 position;
layout(location = 3) in vec4 vertColor;	


layout (std140) uniform Vertex_Data {
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 PV;
} VertData;

out vec4 vVertColor;

void main() {
	//gl_Position = position;
	gl_Position = VertData.PV * VertData.model * position;

	vVertColor = vertColor;
}
