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

layout (std140, binding = 4) uniform Light_Data {
	vec4 pos;
	vec4 lightColor;
} lightData;