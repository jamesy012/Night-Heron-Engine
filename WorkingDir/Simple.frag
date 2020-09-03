#version 450			

#include "Programs/CommonUniforms.inc"
 
in vec2 vTexCoord; 	
in vec4 vVertColor;	
in vec4 vVertNormal;
in vec3 vVertPos; 

in VS_OUT {
    //vec3 FragPos;
    //vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	mat3 TBN;
} fs_in;

out vec4 fragColor; 

layout (binding = 0) uniform sampler2D Diffuse;
layout (binding = 1) uniform sampler2D Normals;
layout (binding = 2) uniform sampler2D Bump;
layout (binding = 3) uniform sampler2D Specular;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir){
	float height_scale = 0.1f;
	float height =  texture(Bump, vTexCoord).r;    
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;    
}

void main() {
	    // offset texture coordinates with Parallax Mapping
    //vec3 viewDir   = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	//vec2 texCoord = ParallaxMapping(vTexCoord, viewDir);
	//if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0){
    //	discard;
	//}

	vec4 texDiffuse = texture(Diffuse, vTexCoord);
	vec4 texNormals = texture(Normals, vTexCoord);
	//vec4 texBump = texture(Bump, vTexCoord);
	vec4 texSpec = texture(Specular, vTexCoord);

	//vec3 lightDir = fs_in.TBN * normalize(lightData.pos.xyz - vVertPos);
   	//vec3 viewDir  = fs_in.TBN * (normalize(cameraData.position - vVertPos).xyz);  

    float ambientStrength = 0.3;
    vec4 ambient = ambientStrength * lightData.lightColor * texDiffuse;

	vec3 norm = normalize(vVertNormal.xyz);
	//vec3 norm = normalize(texNormals * 2.0 - 1.0).xyz;   
	//norm = normalize(vVertNormal.xyz * norm); 
	vec3 lightDir = normalize(lightData.pos.xyz - vVertPos);  

	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = diff * lightData.lightColor * texDiffuse;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(cameraData.position - vVertPos).xyz;
	vec3 reflectDir = reflect(-lightDir, norm);  

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec4 specular = specularStrength * spec * lightData.lightColor * texSpec;  

	vec4 result = (ambient + diffuse + specular) * vVertColor;

	//vec4 result = vec4(1,1,1,1);
	fragColor =result * vVertColor;
	//fragColor =texDiffuse;
} 
