#version 450
#include "Programs/CommonVertexIn.inc"

out vec2 vTexCoord;
out vec4 vVertColor;

out vec4 vVertNormal;
out vec3 vVertPos;


void main() {		
	//gl_Position = projectionViewMatrix * model * position;
	//gl_Position = vertData.projectionViewMatrix * vertData.model * position;
	//gl_Position = vertData.model * position;
	//gl_Position = vertData.projection * vertData.view * vertData.model * position;
	gl_Position = CALC_POSITION;

	vVertPos = vec3(objectData.model * vec4(position));
	vTexCoord = texCoord;
	vVertColor = vertColor;
	mat4 normalMatrix = transpose(inverse(objectData.model));
	vVertNormal = normalMatrix * vertNormal;
}
