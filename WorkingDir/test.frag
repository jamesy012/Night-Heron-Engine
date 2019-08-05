#version 450
 
#include "Programs/CommonUniforms.inc"
#include "Programs/CommonFunctions.inc"

in vec2 vTexCoord; 	
in vec4 vVertColor;	
in vec4 vVertNormal;
						
out vec4 fragColor; 

layout (std140) uniform Shader_Data {
	vec4 color;
} testBlock;

struct LightData {
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 PV;
};

#define NUM_LIGHTS 10
layout (std140) uniform Lighting_Data {
	LightData data[NUM_LIGHTS];
} lightData;

layout (location = 0) uniform sampler2D textureTest;

//uniform vec4 color = vec4(1,1,1,1);  

void main() { 
	//fragColor = vVertColor;
	//fragColor = testBlock.color * vVertColor; 
	//fragColor = testBlock.color; 
	//fragColor = vec4(0,0.5f,0,1);
	for(int i = 0;i<NUM_LIGHTS;i++){
		fragColor.r += lightData.data[i].view[0][0] /255.0f;
		fragColor.g += lightData.data[i].view[0][1] /255.0f;
		fragColor.b += lightData.data[i].view[0][2] /255.0f;
	}

	//fragColor = abs(vVertNormal);
	//return;
//
//
	//float stepRes = 1- step(testBlock.color.x + testBlock.color.y + testBlock.color.z, 2.9f);
//
	//vec2 screenpos = GetScreenPos() * 4;
//
	//screenpos.x += sin(screenpos.y*5 + commonData.time * 0.2f ) * 0.4f * stepRes;
	//screenpos.y += sin(screenpos.x*2 + commonData.time * 0.5f ) * 0.2f * stepRes;
//
//
	////fragColor = texture(textureTest, vTexCoord);
	////fragColor = vec4(vTexCoord.x, vTexCoord.y, 0, 1); 
	////fragColor = vVertColor;
	////fragColor = testBlock.color * texture(textureTest, vTexCoord + vec2(commonData.time * 0.25f,0));
	//fragColor = vVertColor * testBlock.color * texture(textureTest, screenpos + vec2(commonData.time * 0.25f,0));
	////fragColor = testBlock.color * texture(textureTest, vTexCoord);
	////fragColor = texture(textureTest, vTexCoord);
} 
