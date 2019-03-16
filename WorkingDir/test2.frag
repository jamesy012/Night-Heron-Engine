#version 450
 
in vec2 vTexCoord; 	
in vec4 vVertColor;	
						
out vec4 fragColor; 

layout (std140) uniform shader_data {
	vec4 color;
} testBlock;

layout (std140, binding = 1) uniform CommonData {
	float time;
	vec3 pad;
} commonData;

//layout (location = 1) uniform sampler2D textureTest;
 
void main() { 
	fragColor = vVertColor;
} 
