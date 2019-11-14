#version 430

struct VisibleIndex {
	int index;
};

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer{
	VisibleIndex data[];
} visibleLightIndicesBuffer;

layout(std140, binding = 6) uniform LightDebug{
	int totalLightCount;
	int numberOfTilesX;
	int padding;
	int padding2;
} lightDebug;



out vec4 fragColor;

void main() {
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * 64 + tileID.x;

	uint offset = index * 1024;
	uint i;
	for (i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++);

	float ratio = float(i) / float(lightDebug.totalLightCount);
	fragColor = vec4(vec3(ratio, ratio, ratio), 1.0);

/*
	if(lightDebug.totalLightCount + (lightDebug.numberOfTilesX-64) > 512){
		fragColor = vec4(1,0,0,1);
	}else{
		fragColor = vec4(0,0,1,1);
	}
	*/
}
