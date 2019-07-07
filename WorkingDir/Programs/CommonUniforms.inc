layout (std140) uniform Vertex_Data {
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 PV;
} vertData;

layout (std140, binding = 1) uniform Common_Data {
	float time;
	vec2 screenSize;
	float pad;
} commonData;
