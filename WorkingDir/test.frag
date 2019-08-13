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

struct LightData {
	vec3 pos;
	float ambientStrength;
	vec3 color;
	float specularStrength;
};

#define NUM_LIGHTS 1
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
	// for(int i = 0;i<NUM_LIGHTS;i++){
	// 	fragColor.r += lightData.data[i].view[0][0] /255.0f;
	// 	fragColor.g += lightData.data[i].view[0][1] /255.0f;
	// 	fragColor.b += lightData.data[i].view[0][2] /255.0f;
	// }

    vec3 ambient = lightData.data[0].ambientStrength * lightData.data[0].color;

	vec3 norm = normalize(vVertNormal.xyz);
	vec3 lightDir = normalize(lightData.data[0].pos - vVertPos.xyz);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightData.data[0].color;
	//vec3 result = (ambient + diffuse) * testBlock.color.xyz;

	vec3 viewDir = normalize(cameraData.position - vVertPos.xyz);
	vec3 reflectDir = reflect(-lightDir, norm); 

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = lightData.data[0].specularStrength * spec * lightData.data[0].color;  

	vec3 result = (ambient + diffuse + specular) * testBlock.color.xyz;
	fragColor = vec4(result, 1.0);
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
