#version 450
 
in vec2 vTexCoord; 	
in vec4 vVertColor;	
						
out vec4 fragColor; 

layout (std140) uniform Shader_Data {
	vec4 color;
} testBlock;

layout (std140, binding = 1) uniform Common_Data {
	float time;
	vec3 pad;
} commonData;

layout (location = 0) uniform sampler2D textureTest;
 
void main() { 
	fragColor = texture(textureTest, vTexCoord);
} 
