#pragma once
#include "Debug.h"

#include "ShaderManager.h"

#include <glm\glm.hpp>


struct PointLight {
	glm::vec3 pos;
	float ambientStrength;
	glm::vec3 color;
	float specularStrength;
};

#define MAX_NUM_LIGHTS 1
CREATE_BUFFER_UNIFORM(PointLightsData,
					  PointLight data[MAX_NUM_LIGHTS];
)

class LightManager {
public:
	LightManager();
	~LightManager();


};