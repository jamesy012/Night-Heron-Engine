#version 450
 
#include "Programs/CommonUniforms.inc"
#include "Programs/CommonFunctions.inc"

in vec2 vTexCoord; 	
in vec4 vVertColor;	
in vec4 vVertNormal;
in vec3 vVertPos; 	

layout(location=5)in vec4 vFragDirLightSpace;

layout(location=7) in ShadowMatrix{
	vec4 FragDirLightSpace[NUM_SHADOWS];
} vShadowMatrix;

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
	
	vec4 UVOffsets;
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

	vec4 UVOffsets;	
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
#define COLORED_SHADOW 1

layout (std140) uniform Lighting_Data {
	PointLight pointLights[NUM_POINTLIGHTS];
	DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];
	SpotLight spotLights[NUM_SPOT_LIGHTS];
} lightData;

layout (binding = 0) uniform sampler2D textureTest;
layout (binding = 1) uniform sampler2D shadowTexture;

float LinearizeDepth(float depth, float near, float far)
{
    //float z = depth * 2.0 - 1.0; // Back to NDC 
    //return (2.0 * near * far) / (far + near - z * (far - near));
	return near * far / (far + depth * (near - far));
}

float ShadowCalculation(vec4 dirShadowSpace, vec3 lightDir, vec4 UVOffset)
{
	// perform perspective divide
	vec3 projCoords = dirShadowSpace.xyz / dirShadowSpace.w;
	//return projCoords.z;
	//projCoords *= vec3(-1,1,1);

	// transform to [0,1] range
	projCoords.x = projCoords.x * 0.5 + 0.5;
    projCoords.y = projCoords.y * 0.5 + 0.5;
	#if WITH_OPENGL
		projCoords.z = projCoords.z * 0.5 + 0.5;
    #else
		projCoords.z = projCoords.z;
	#endif

	vec2 shadowUV = projCoords.xy;
	shadowUV.x = UVOffset.x + (shadowUV.x * UVOffset.z);
	shadowUV.y = UVOffset.y + (shadowUV.y * UVOffset.w);

	if(shadowUV.x > UVOffset.x + UVOffset.z || shadowUV.x < UVOffset.x){
		return 0;
	}
	if(shadowUV.y > UVOffset.y + UVOffset.w || shadowUV.y < UVOffset.y){
		return 0;
	}
	#if WITH_DIRECTX
		shadowUV.y = (shadowUV.y *-1 + UVOffset.w);
	#endif
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowTexture, shadowUV).r; 
	
	//closestDepth = LinearizeDepth(closestDepth,1,200);

	//return closestDepth;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	//return currentDepth;
	//vec3 lightDirNorm = normalize(lightDir);
	//float bias = max(0.05 * (1.0 - dot(vVertNormal.xyz, lightDirNorm)), 0.005); 
	float margin = acos(dot(vVertNormal.xyz, lightDir));
	float bias = 0.0005/margin;
	bias = clamp(bias, 0, .1);

    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;  

	if(currentDepth > 1.0) {
        shadow = 0.0;
	}

	return shadow;
}
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

#if COLORED_SHADOW
	vec3 shadow = vec3(0);
#else
float shadow = 1;
#endif
	for(int i =0;i<NUM_DIRECTIONAL_LIGHTS;i++){
		float thisShadow = ShadowCalculation(
		vShadowMatrix.FragDirLightSpace[i],
		 lightData.directionalLights[i].direction,
		 lightData.directionalLights[i].UVOffsets);
#if COLORED_SHADOW	 
		shadow += (1-thisShadow) * lightData.directionalLights[i].color;
#else
		shadow += (1-thisShadow);
#endif
	}

	for(int i =0;i<NUM_SPOT_LIGHTS;i++){
		float thisShadow = ShadowCalculation(
		vShadowMatrix.FragDirLightSpace[2+i],
		 lightData.spotLights[i].direction,
		 lightData.spotLights[i].UVOffsets);

		vec3 lightDir = normalize(lightData.spotLights[i].position - vVertPos.xyz);  
		float theta = dot(lightDir, normalize(-lightData.spotLights[i].direction));		
		thisShadow = thisShadow * step(lightData.spotLights[i].cutOff, theta); 
#if COLORED_SHADOW	 
		shadow += (1-thisShadow) * lightData.spotLights[i].color;
#else
		 shadow += (1-thisShadow);
#endif
	}


	//fragColor = vec4(shadow, shadow, shadow, 1);
	//fragColor = vec4(shadow, 1);
	//fragColor = vFragDirLightSpace;
	//return;
	{		

		vec3 finalSpecular;
		vec3 finalAmbient;
		vec3 finalDiffuse;

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
			//result += (diffuse + specular) * attenuation;
			finalDiffuse += diffuse * attenuation;
			finalSpecular += specular * attenuation;
		}
		//Directional
		for(int i =0;i<NUM_DIRECTIONAL_LIGHTS;i++) {

			DirectionalLight light = lightData.directionalLights[i];

			vec3 lightDir = normalize(light.direction);

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

			//result += (ambient + diffuse + specular);
			finalAmbient += ambient;
			finalDiffuse += diffuse;
			finalSpecular += specular;
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
				//result += (diffuse + specular) * attenuation;
				finalDiffuse += diffuse * attenuation;
				finalSpecular += specular * attenuation;
			}
			//else  // else, use ambient light so scene isn't completely dark outside the spotlight.
			//color = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);
		}
		result = finalAmbient + (finalDiffuse + finalSpecular) * ( shadow);
	}
	
	//vec3 result = (ambient + diffuse + specular) * testBlock.color.xyz;
	//result *= testBlock.color.xyz;
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
