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

//LIGHTING
struct DirectionalLight {
	vec3 direction;
	float ambientStrength;
	vec3 color;
	float specularStrength;
};

struct SpotLight {
    vec3  position;
    float cutOff;

    vec3  direction;
	float linearVal;

	vec3 color;
	float constant;

	float quadratic;
	float shininess;
	float specularStrength;
	float pad;
};    

struct PointLight {
	vec3 position;
	float ambientStrength;
	vec3 color;
	float specularStrength;

	float constant;
	float linearVal;
	float quadratic;
	float shininess;
};

#define BLINN_PHONG 1

#define NUM_POINTLIGHTS 2
#define NUM_DIRECTIONAL_LIGHTS 1
#define NUM_SPOT_LIGHTS 1
layout (std140) uniform Lighting_Data {
	PointLight pointLights[NUM_POINTLIGHTS];
	DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];
	SpotLight spotLights[NUM_SPOT_LIGHTS];
} lightData;

layout (location = 0) uniform sampler2D textureTest;

//uniform vec4 color = vec4(1,1,1,1);  

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

	//lighting
	vec3 result = vec3(0);
	{
		vec3 norm = normalize(vVertNormal.xyz);

		//pointlights
		for(int i =0;i<NUM_POINTLIGHTS;i++) {

			PointLight light = lightData.pointLights[i];

			vec3 lightDir = normalize(light.position - vVertPos.xyz);  

			//diffuse
			float diff = max(dot(norm, lightDir), 0.0);

			//specular
#if BLINN_PHONG == 1
			vec3 viewDir    = normalize(cameraData.position - vVertPos.xyz);
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(norm, halfwayDir), 0.0), light.shininess);
#else
			vec3 viewDir = normalize(cameraData.position - vVertPos.xyz);
			vec3 reflectDir = reflect(-lightDir, norm); 

			float spec = pow(max(dot(viewDir, reflectDir), 0.0), light.shininess);
#endif

			//falloff
			float distance    = length(light.position - vVertPos.xyz);
			float attenuation = 1.0 / (light.constant + light.linearVal * distance + light.quadratic * (distance * distance));    


			//vec3 ambient = light.ambientStrength * light.color;
			vec3 diffuse = diff * light.color;
			vec3 specular = light.specularStrength * spec * light.color;  

			//result += (ambient + diffuse + specular) * attenuation;
			result += (diffuse + specular) * attenuation;
		}
		//Directional
		for(int i =0;i<NUM_DIRECTIONAL_LIGHTS;i++) {

			DirectionalLight light = lightData.directionalLights[i];

			vec3 lightDir = normalize(-light.direction);

			//diffuse
			float diff = max(dot(norm, lightDir), 0.0);

			//specular
#if BLINN_PHONG == 1
			vec3 viewDir    = normalize(cameraData.position - vVertPos.xyz);
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(norm, halfwayDir), 0.0), 32);
#else
			vec3 viewDir = normalize(cameraData.position - vVertPos.xyz);
			vec3 reflectDir = reflect(-lightDir, norm); 

			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
#endif


			vec3 ambient = light.ambientStrength * light.color;
			vec3 diffuse = diff * light.color;
			vec3 specular = light.specularStrength * spec * light.color;  

			result += (ambient + diffuse + specular);
		}

		//SpotLight

		for(int i =0;i<NUM_SPOT_LIGHTS;i++) {
			SpotLight light = lightData.spotLights[i];

			vec3 lightDir = normalize(light.position - vVertPos.xyz);  

			float theta = dot(lightDir, normalize(-light.direction));
		
			if(theta > light.cutOff) 
			{ 
				//diffuse
				float diff = max(dot(norm, lightDir), 0.0);

				//specular
	#if BLINN_PHONG == 1
				vec3 viewDir    = normalize(cameraData.position - vVertPos.xyz);
				vec3 halfwayDir = normalize(lightDir + viewDir);
				float spec = pow(max(dot(norm, halfwayDir), 0.0), 32);
	#else
				vec3 viewDir = normalize(cameraData.position - vVertPos.xyz);
				vec3 reflectDir = reflect(-lightDir, norm); 

				float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	#endif

				//falloff
				float distance    = length(light.position - vVertPos.xyz);
				float attenuation = 1.0 / (1 + light.linearVal * distance + light.quadratic * (distance * distance));    


				//vec3 ambient = light.ambientStrength * light.color;
				vec3 diffuse = diff * light.color;
				vec3 specular = light.specularStrength * spec * light.color;  

				//result += (ambient + diffuse + specular) * attenuation;
				result += (diffuse + specular) * attenuation;
			}
			//else  // else, use ambient light so scene isn't completely dark outside the spotlight.
			//color = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);
		}

	}
	
	//vec3 result = (ambient + diffuse + specular) * testBlock.color.xyz;
	result * testBlock.color.xyz;
	//fragColor = vec4(result, 1.0);
	//return;
	fragColor = texture(textureTest, vTexCoord) * vec4(result, 1.0);
/*
	float stepRes = 1- step(testBlock.color.x + testBlock.color.y + testBlock.color.z, 2.9f);
	stepRes = 1;
	
	vec2 screenpos = GetScreenPos() * 4;

	screenpos.x += sin(screenpos.y*5 + commonData.time * 0.2f ) * 0.4f * stepRes;
	screenpos.y += sin(screenpos.x*2 + commonData.time * 0.5f ) * 0.2f * stepRes;
*/

	//fragColor = texture(textureTest, vTexCoord);
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
