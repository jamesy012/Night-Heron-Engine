#version 450
#include "Programs/CommonVertexIn.inc"

out vec2 vTexCoord;
out vec4 vVertColor;

out vec4 vVertNormal;
out vec3 vVertPos;

layout(location=4) out vec2 vPosTest;


layout(location=7) out ShadowMatrix{
	vec4 FragDirLightSpace[4];
} vShadowMatrix;

layout (std140, binding = 4) uniform Shadow_Data {
	mat4 DirLightPV[2];
} shadowData;


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
	
	for(int i = 0;i<4;i++){
		vShadowMatrix.FragDirLightSpace[i] = shadowData.DirLightPV[i] * vec4(vVertPos, 1.0);
	}

	vec3 ndc = gl_Position.xyz / gl_Position.w;
	vPosTest = ndc.xy * 0.5 + 0.5;
	vPosTest *= vec2(1,-1);

}
