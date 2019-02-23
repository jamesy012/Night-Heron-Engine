#version 450
 
in vec2 vTexCoord; 	
in vec4 vVertColor;	
						
out vec4 fragColor; 
 
//uniform vec4 color = vec4(1,1,1,1); 

layout (std140) uniform shader_data {
	vec4 color;
} testBlock;

layout (location = 0) uniform sampler2D textureTest;
 
void main() { 
	//fragColor = vVertColor;
	//fragColor = testBlock.color * vVertColor; 
	//fragColor = testBlock.color; 
	
	fragColor = texture(textureTest, vTexCoord);
	//fragColor = vec4(vTexCoord.x, vTexCoord.y, 0, 1); 
	//fragColor = vVertColor;
	//fragColor = testBlock.color * texture(textureTest, vTexCoord);
} 
