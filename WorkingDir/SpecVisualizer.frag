#version 450			

#include "Programs/CommonUniforms.inc"
 
in vec2 vTexCoord; 	
in vec4 vVertColor;	
in vec4 vVertNormal;
in vec3 vVertPos; 

out vec4 fragColor; 

layout (binding = 0) uniform sampler2D Diffuse;
layout (binding = 3) uniform sampler2D Specular;

void main() {

	vec4 texDiffuse = texture(Diffuse, vTexCoord);
	vec4 texSpec = texture(Specular, vTexCoord);

    float ambientStrength = 0.3;
    vec4 ambient = ambientStrength * lightData.lightColor * texSpec;

	vec3 norm = normalize(vVertNormal.xyz);
	vec3 lightDir = normalize(lightData.pos.xyz - vVertPos);  

	//float diff = max(dot(norm, lightDir), 0.0);
	//vec4 diffuse = diff * lightData.lightColor * texSpec;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(cameraData.position - vVertPos).xyz;
	vec3 reflectDir = reflect(-lightDir, norm);  

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec4 specular = specularStrength * spec * lightData.lightColor * texSpec;  

	vec4 result = (ambient + /*diffuse +*/ specular);

	//vec4 result = vec4(1,1,1,1);
	fragColor = result * vVertColor;
} 
