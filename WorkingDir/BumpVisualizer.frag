#version 450			

#include "Programs/CommonUniforms.inc"

in vec2 vTexCoord; 	
in vec4 vVertColor;	
in vec4 vVertNormal;
in vec3 vVertPos; 

out vec4 fragColor; 

layout (binding = 0) uniform sampler2D Diffuse;
layout (binding = 2) uniform sampler2D Bump;

void main() {

	vec4 texDiffuse = texture(Diffuse, vTexCoord);
	vec4 texBump = texture(Bump, vTexCoord);

    float ambientStrength = 0.05;
    vec4 ambient = ambientStrength * lightData.lightColor * texDiffuse;

	vec4 diffuse = texBump;

	vec4 result = (ambient + diffuse);

	//vec4 result = vec4(1,1,1,1);
	fragColor = result * vVertColor;
} 
