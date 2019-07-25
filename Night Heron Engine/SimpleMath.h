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

	union {
		float v1;
		float x;
	};
	union {
		float v2;
		float y;
	};
};

struct SimpleVec3 {
public:
	SimpleVec3(float i1, float i2, float i3) {
		v1 = i1;
		v2 = i2;
		v3 = i3;
	}

	union {
		float v1;
		float x;
	};
	union {
		float v2;
		float y;
	};	
	union {
		float v3;
		float z;
	};
};
