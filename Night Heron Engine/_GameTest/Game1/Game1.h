#pragma once

#include "AppBase.h"

#include "Scene.h"
#include "FlyCamera.h"

#include "Managers/ShaderManager.h"
#include "nlohmann/json.hpp"

class ShaderUniformBlock;
class Model;

CREATE_BUFFER_UNIFORM(TestUniformStruct2,
					  glm::vec4 Color;
)

CREATE_BUFFER_UNIFORM(CommonDataStruct,
					  float time;
glm::vec2 screenSize;
float pad;
)

CREATE_BUFFER_UNIFORM_WITH_JSON(LightDataStruct,
								glm::vec4 lightPos;
glm::vec4 lightColor;
glm::vec3 padding;
)


class Game1 : public AppBase {
public:
	void Start() override;
	void Update() override;
	void ImguiDraw() override;
	void Draw() override;
	void Finish() override;
private:
	FlyCamera mainCamera;
	Scene scene;


	Shader* normalShader;
	Shader* SpecShader;
	Shader* BumpShader;


	CommonDataStruct commonPerFrameData;
	CameraUniformStruct cameraUniformData;
	ObjectUniformStruct ObjectUniformStructObj;
	LightDataStruct lightData;
	TestUniformStruct2 colorTest;

	ShaderUniformBlock* cameraUniformBlock;
	ShaderUniformBlock* testUniform;
	ShaderUniformBlock* commonDataBlock;
	ShaderUniformBlock* lightDataBlock;
	ShaderUniformBlock* testUniform2;

	Model* testModel;
	Model* groundModel;
	Model* pathModel;

	float fov = 90;

};

