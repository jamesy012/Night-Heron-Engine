layout(location = 0) in vec4 position;
layout(location = 1) in vec4 vertNormal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec4 vertColor;	

/*
layout (std140, binding = 0) uniform Vertex_Data {
	//mat4 view;
	//mat4 projection;
	mat4 PV;
} vertData;
*/

layout (std140, binding = 1) uniform Object_Data {
	mat4 model;
} objectData;

#include "CommonUniforms.inc"

#define CALC_POSITION cameraData.PV * objectData.model * position
