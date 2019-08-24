#version 450
#include "Programs/CommonVertexIn.inc"

#include "Programs/CommonUniforms.inc"

out vec2 vTexCoord;
out vec4 vVertColor;
out vec4 vVertNormal;

void main() {		
	float treeSpeed = commonData.time * 2;
	vec3 offset = vec3(sin(position.x + treeSpeed),sin(position.y + treeSpeed) * 0.25f, cos(position.z + treeSpeed * 1.222f)) * 0.2f;
	gl_Position = cameraData.PV * objectData.model * (position + vec4(offset,0));

	vTexCoord = texCoord;
	vVertColor = vertColor;
	mat4 normalMatrix = transpose(inverse(objectData.model));
	vVertNormal = normalMatrix * vertNormal;
}
