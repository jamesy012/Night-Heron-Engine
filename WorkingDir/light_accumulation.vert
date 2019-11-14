#version 450

#include "Programs/CommonVertexIn.inc"
#include "Programs/CommonUniforms.inc"

out VERTEX_OUT {
	vec3 fragmentPosition;
	vec2 textureCoordinates;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
	vec3 normal;
} vertex_out;

void main() {

	gl_Position = CALC_POSITION;

	vertex_out.fragmentPosition = vec3(objectData.model * position);
	vertex_out.textureCoordinates = texCoord;

	mat3 normalMatrix = transpose(inverse(mat3(objectData.model)));
	vec3 tan = normalize(normalMatrix * tangent);
	vec3 bitan = normalize(normalMatrix * bitangent);
	vec3 norm = normalize(normalMatrix * vertNormal.xyz);

	// For tangent space normal mapping
	mat3 TBN = transpose(mat3(tan, bitan, norm));
	vertex_out.tangentViewPosition = TBN * cameraData.position;
	vertex_out.tangentFragmentPosition = TBN * vertex_out.fragmentPosition;
	vertex_out.TBN = TBN;

	vertex_out.normal = norm;
}
