layout (std140, binding = 2) uniform Common_Data {
	float time;
	vec2 screenSize;
	float pad;
} commonData;

layout (std140, binding = 3) uniform Camera_Data {
	vec3 position;
	float pad;
	mat4 PV;
} cameraData;
