#version 450
 
#include "Programs/CommonUniforms.inc"

in vec2 vTexCoord; 	
in vec4 vVertColor;	
in vec4 vVertNormal;

						
out vec4 fragColor; 

layout (std140) uniform Shader_Data {
	vec4 color;
} testBlock;


//layout (location = 1) uniform sampler2D textureTest;
 
void main() { 
	fragColor = vVertColor;
	fragColor = (vVertNormal);
} 
