#version 450
#include "Programs/CommonVertexIn.inc"

#include "Programs/CommonUniforms.inc"

out vec2 vTexCoord;
out vec4 vVertColor;

out vec4 vVertNormal;
out vec3 vVertPos;

void main() {		
	//gl_Position = projectionViewMatrix * model * position;
	//gl_Position = vertData.projectionViewMatrix * vertData.model * position;
	//gl_Position = vertData.model * position;
	//gl_Position = vertData.projection * vertData.view * vertData.model * position;
	gl_Position = vertData.PV * vertData.model * position;

	vVertPos = vec3(vertData.model * vec4(position));
	vTexCoord = texCoord;
	vVertColor = vertColor;
		mat4 normalMatrix = transpose(inverse(vertData.model));
	vVertNormal = normalMatrix * vertNormal;
}
