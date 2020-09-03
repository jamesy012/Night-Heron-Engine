#version 450
 
#include "Programs/CommonUniforms.inc"
#include "Programs/CommonFunctions.inc"

in vec2 vTexCoord; 	
in vec4 vVertColor;	
in vec4 vVertNormal;
in vec3 vVertPos; 	

out vec4 fragColor; 

layout (std140) uniform Shader_Data {
	vec4 color;
} testBlock;

layout (binding = 0) uniform sampler2D textureTest;

void main() { 
	//fragColor = vVertColor;
	//fragColor = testBlock.color * vVertColor; 
	//fragColor = testBlock.color; 
	//fragColor = vec4(0,0.5f,0,1);
	// for(int i = 0;i<NUM_POINTLIGHTS;i++){
	// 	fragColor.r += lightData.pointLights[i].view[0][0] /255.0f;
	// 	fragColor.g += lightData.pointLights[i].view[0][1] /255.0f;
	// 	fragColor.b += lightData.pointLights[i].view[0][2] /255.0f;
	// }

	fragColor = texture(textureTest, vTexCoord);
	//fragColor = vec4(vTexCoord.x, vTexCoord.y, 0, 1); 
	//fragColor = vVertColor;
	//fragColor = testBlock.color * texture(textureTest, vTexCoord + vec2(commonData.time * 0.25f,0));
	//fragColor = vVertColor * testBlock.color * texture(textureTest, screenpos + vec2(commonData.time * 0.25f,0));
	//fragColor = vVertColor * testBlock.color * texture(textureTest, vTexCoord);
	//fragColor = testBlock.color * texture(textureTest, vTexCoord);
	//fragColor = texture(textureTest, vTexCoord);
	//fragColor = vec4(GetScreenPos().xy, 0,1);
	//fragColor = vec4(screenpos, 0,1);
} 
