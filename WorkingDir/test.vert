#version 450
#include "Programs/testInclude.inc"


layout (std140) uniform Vertex_Data {
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 PV;
} VertData;

layout (std140, binding = 1) uniform CommonData {
	float time;
	vec3 pad;
} commonData;

out vec2 vTexCoord;
out vec4 vVertColor;

void main() {		
	//gl_Position = projectionViewMatrix * model * position;
	//gl_Position = VertData.projectionViewMatrix * VertData.model * position;
	//gl_Position = VertData.model * position;
	//gl_Position = VertData.projection * VertData.view * VertData.model * position;
	gl_Position = VertData.PV * VertData.model * position;

	vTexCoord = texCoord;
	vVertColor = vertColor;
}
