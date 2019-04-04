#pragma once
//Basic Data holders

typedef unsigned int uint;
typedef unsigned char uchar;

struct SimpleVec2 {
public:
	SimpleVec2(float i1, float i2) {
		v1 = i1;
		v2 = i2;
	}

	float v1;
	float v2;
};

struct SimpleVec3 {
public:
	SimpleVec3(float i1, float i2, float i3) {
		v1 = i1;
		v2 = i2;
		v3 = i3;
	}

	float v1;
	float v2;
	float v3;
};
