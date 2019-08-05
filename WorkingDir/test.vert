#version 450
#include "Programs/CommonVertexIn.inc"

#include "Programs/CommonUniforms.inc"

out vec2 vTexCoord;
out vec4 vVertColor;
out vec4 vVertNormal;

void main() {		
	//gl_Position = projectionViewMatrix * model * position;
	//gl_Position = vertData.projectionViewMatrix * vertData.model * position;
	//gl_Position = vertData.model * position;
	//gl_Position = vertData.projection * vertData.view * vertData.model * position;
	gl_Position = vertData.PV * vertData.model * position;

	vTexCoord = texCoord;
	vVertColor = vertColor;
	vVertNormal = vertNormal;
}
