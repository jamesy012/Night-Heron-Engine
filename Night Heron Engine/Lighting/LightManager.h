#pragma once
#include "Debug.h"

#include "Managers/ShaderManager.h"

#include <glm\glm.hpp>


struct PointLight {
	glm::vec3 pos;
	float ambientStrength;
	glm::vec3 color;
	float specularStrength;

	float constant;
	float linear;
	float quadratic;
	float shininess;

	PointLight() {
		pos = glm::vec3(0);
		ambientStrength = 0.2f;
		color = glm::vec3(0);
		specularStrength = 0.5f;

		constant = 1.0f;//typically 1
		//linear = 0.09f;
		//quadratic = 0.032f;
		linear = 0.045f;
		quadratic = 0.00075f;
		shininess = 32;
	}
};

struct DirectionalLight {
	glm::vec3 direction;
	float ambientStrength;
	glm::vec3 color;
	float specularStrength;

	DirectionalLight() {
		direction = glm::vec3(0, -1, 0);
		ambientStrength = 0.2f;
		color = glm::vec3(0);
		specularStrength = 0.5f;
	}
};

struct SpotLight {
	glm::vec3  pos;
	float cutOff;
	glm::vec3  direction;
	float linear;

	glm::vec3 color;
	float constant;

	float quadratic;
	float shininess;
	float specularStrength;
	float pad;

	SpotLight() {
		pos = glm::vec3(0);
		cutOff = glm::cos(glm::radians(12.5f));
		direction = glm::vec3(0,-1,0);
		color = glm::vec3(0);


		constant = 1.0f;//typically 1
		//linear = 0.09f;
		//quadratic = 0.032f;
		linear = 0.045f;
		quadratic = 0.00075f;
		shininess = 32;

		specularStrength = 0.5f;
	}
};

#define MAX_NUM_POINTLIGHTS 2
#define MAX_NUM_DIRECTIONALLIGHTS 1
#define MAX_NUM_SPOTLIGHTS 1
CREATE_BUFFER_UNIFORM(LightsData,
	PointLight pointLights[MAX_NUM_POINTLIGHTS];
	DirectionalLight directionalLights[MAX_NUM_DIRECTIONALLIGHTS];
	SpotLight spotLights[MAX_NUM_SPOTLIGHTS];
)

class LightManager {
public:
	LightManager();
	~LightManager();


};