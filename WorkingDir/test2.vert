#version 450
layout(location = 0) in vec4 position;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec4 vertColor;	

layout (std140, binding = 0) uniform Vertex_Data {
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 PV;
} vertData;

layout (std140, binding = 1) uniform Common_Data {
	float time;
	vec3 pad;
} commonData;

out vec2 vTexCoord;
out vec4 vVertColor;

void main() {		
	float treeSpeed = commonData.time * 2;
	vec3 offset = vec3(sin(position.x + treeSpeed),sin(position.y + treeSpeed) * 0.25f, cos(position.z + treeSpeed * 1.222f)) * 0.2f;
	gl_Position = vertData.PV * vertData.model * (position + vec4(offset,0));

	vTexCoord = texCoord;
	vVertColor = vertColor;
}
