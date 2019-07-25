#version 450
 
#include "Programs/CommonUniforms.inc"
#include "Programs/CommonFunctions.inc"

in vec2 vTexCoord; 	
in vec4 vVertColor;	
						
out vec4 fragColor; 

layout (std140) uniform shader_data {
	vec4 color;
} testBlock;

layout (location = 0) uniform sampler2D textureTest;

//uniform vec4 color = vec4(1,1,1,1);  

void main() { 
	//fragColor = vVertColor;
	//fragColor = testBlock.color * vVertColor; 
	//fragColor = testBlock.color; 

	float stepRes = 1- step(testBlock.color.x + testBlock.color.y + testBlock.color.z, 2.9f);

	vec2 screenpos = GetScreenPos() * 4;

	screenpos.x += sin(screenpos.y*5 + commonData.time * 0.2f ) * 0.4f * stepRes;
	screenpos.y += sin(screenpos.x*2 + commonData.time * 0.5f ) * 0.2f * stepRes;


	//fragColor = texture(textureTest, vTexCoord);
	//fragColor = vec4(vTexCoord.x, vTexCoord.y, 0, 1); 
	//fragColor = vVertColor;
	//fragColor = testBlock.color * texture(textureTest, vTexCoord + vec2(commonData.time * 0.25f,0));
	fragColor = vVertColor * testBlock.color * texture(textureTest, screenpos + vec2(commonData.time * 0.25f,0));
	//fragColor = testBlock.color * texture(textureTest, vTexCoord);
	//fragColor = texture(textureTest, vTexCoord);
} 
