#version 450
#include "Programs/CommonVertexIn.inc"

#include "Programs/CommonUniforms.inc"


out vec2 vTexCoord;
out vec4 vVertColor;

out vec4 vVertNormal;
out vec3 vVertPos;

out VS_OUT {
    //vec3 FragPos;
    //vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	mat3 TBN;
} vs_out;

void main() {		
	//gl_Position = projectionViewMatrix * model * position;
	//gl_Position = vertData.projectionViewMatrix * vertData.model * position;
	//gl_Position = vertData.model * position;
	//gl_Position = vertData.projection * vertData.view * vertData.model * position;
	gl_Position = CALC_POSITION;

	vVertPos = vec3(objectData.model * vec4(position));
	vTexCoord = texCoord;
	vVertColor = vertColor;
	mat4 normalMatrix = transpose(inverse(objectData.model));
	vVertNormal = normalMatrix * vertNormal;

	vec3 T   = normalize(mat3(objectData.model) * tangent);
    vec3 B   = normalize(mat3(objectData.model) * biTangent);
    vec3 N   = normalize(mat3(objectData.model) * vertNormal.xyz);
    mat3 TBN = transpose(mat3(T, B, N));

	vs_out.TBN = mat3(T, B, N);
    vs_out.TangentLightPos = TBN * lightData.pos.xyz;
    vs_out.TangentViewPos  = TBN * cameraData.position.xyz;
    vs_out.TangentFragPos  = TBN * vVertPos.xyz;
}
