#include "Programs/CommonVertexIn.inc"

#include "Programs/CommonUniforms.inc"

out vec4 vVertColor;

void main() {
	//gl_Position = position;
	gl_Position = vertData.PV * vertData.model * position;

	vVertColor = vertColor;
}
