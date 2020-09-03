#include "stdafx.h"
#include "Game1.h"

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

#include "Graphics/API/GFXAPI.h"

#include "Graphics/API/Shader.h"
#include "Graphics/API/Texture.h"
#include "Graphics/API/RenderTarget.h"
#include "Window.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"
#include "Util.h"
#include "FlyCamera.h"

#include "Managers/ShaderManager.h"
#include "Managers/ShaderSpirvManager.h"
#include "Managers/TextureManager.h"
#include "Managers/Manager.h"
#include "Managers/Arguments.h"
#include "Singletons.h"

#include "Managers/TimeManager.h"

#include "Managers/IniFile.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include "jsonConverter/JCglm.h"

#include <Dependency/ImGui/imgui.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "Graphics/ShaderSpirvData.h"

#include "Transform.h"
#include "Object.h"
#include "Scene.h"

#include "tests/ObjectDrawTest.h"

#include "Input/InputHandler.h"

#include "nlohmann/json.hpp"

void to_json(nlohmann::json& j, const LightDataStruct& p) {
	j = nlohmann::json{ { "Color", p.lightColor }, { "Pos", p.lightPos } };
}

void from_json(const nlohmann::json& j, LightDataStruct& p) {
	j.at("Color").get_to(p.lightColor);
	j.at("Pos").get_to(p.lightPos);
}


void Game1::Start() {
	//SHADERS
	Shader* testShader = _CShaderManager->GetShader("Shaders/TestShader.shader");
	if (testShader == nullptr) {
		testShader = _CGraphics->CreateShader();
		testShader->m_FilePath = "Shaders/TestShader.shader";

		if (!testShader->Load()) {
			testShader->SetDebugObjName("Test Shader");
			testShader->m_ShouldPrintCode = true;
			testShader->AddShader(_CShaderSpirvManager->GetShaderPart("test.vert"));
			testShader->AddShader(_CShaderSpirvManager->GetShaderPart("test.frag"));
			testShader->LinkShaders();

			testShader->Save();
		}
	}

	normalShader = _CShaderManager->GetShader("Simple Shader.shader");

	SpecShader = _CShaderManager->GetShader("Shaders/SpecShader.shader");
	if (SpecShader == nullptr) {
		SpecShader = _CGraphics->CreateShader();
		SpecShader->m_FilePath = "Shaders/SpecShader.shader";

		if (!SpecShader->Load()) {
			SpecShader->SetDebugObjName("Spec Shader");
			SpecShader->m_ShouldPrintCode = true;
			SpecShader->AddShader(_CShaderSpirvManager->GetShaderPart("Simple.vert"));
			SpecShader->AddShader(_CShaderSpirvManager->GetShaderPart("SpecVisualizer.frag"));
			SpecShader->LinkShaders();

			SpecShader->Save();
			_CShaderManager->AddShader(SpecShader);
		}
	}

	BumpShader = _CShaderManager->GetShader("Shaders/BumpShader.shader");
	if (BumpShader == nullptr) {
		BumpShader = _CGraphics->CreateShader();
		BumpShader->m_FilePath = "Shaders/BumpShader.shader";

		if (!BumpShader->Load()) {
			BumpShader->SetDebugObjName("Bump Shader");
			BumpShader->m_ShouldPrintCode = true;
			BumpShader->AddShader(_CShaderSpirvManager->GetShaderPart("Simple.vert"));
			BumpShader->AddShader(_CShaderSpirvManager->GetShaderPart("BumpVisualizer.frag"));
			BumpShader->LinkShaders();

			BumpShader->Save();
			_CShaderManager->AddShader(BumpShader);
		}
	}

	//MATERIALS
	//Material SimpleMaterial("Material/SimpleMat.mat");
	//if (!SimpleMaterial.Load()) {
	//	SimpleMaterial.SetDebugObjName("Simple");
	//	SimpleMaterial.m_Shader = _CShaderManager->GetShader("Simple Shader.shader");
	//	SimpleMaterial.Save();
	//}

	//Assimp::DefaultLogger::create();

	//MODELS
	Model* squareModel = new Model();
	squareModel->CreateSquare();

	Model* planeModel = new Model();
	planeModel->CreatePlane();

	//Model testBistro = Model();
	//testBistro.LoadModel("Models/Bistro v4 Update/Bistro_v4/Bistro_Interior.fbx");

	testModel = new Model();
	groundModel = new Model();
	pathModel = new Model();
	//testModel.LoadModel("Models/sponza/sponza.obj");
	//testModel.LoadModel("Models/sponzaDetailed/sponza.obj");
	testModel->LoadModel("Models/syntystudios_Dont upload/SM_Env_Tree_01.fbx");
	groundModel->LoadModel("Models/syntystudios_Dont upload/SM_Env_Path_Dirt_01.fbx");
	pathModel->LoadModel("Models/syntystudios_Dont upload/SM_Env_Path_Tile_01.fbx");
	//testModel.LoadModel("Models/nanosuit.obj");

	//testModel.SetMaterial(&SimpleMaterial, 0);

	//UNIFORMS
	cameraUniformBlock = _CGraphics->CreateBuffer(&cameraUniformData, sizeof(CameraUniformStruct));
	cameraUniformBlock->SetDebugObjName("Camera Data");
	_CManager->RegisterShaderUniform(cameraUniformBlock, "Camera_Data");

	testUniform = _CGraphics->CreateBuffer(&ObjectUniformStructObj, sizeof(ObjectUniformStruct));
	testUniform->SetDebugObjName("MVP Buffer");
	_CManager->RegisterShaderUniform(testUniform, "Object_Data");


	commonDataBlock = _CGraphics->CreateBuffer(&commonPerFrameData, sizeof(CommonDataStruct));
	commonDataBlock->SetDebugObjName("Common Data Buffer");
	_CManager->RegisterShaderUniform(commonDataBlock, "Common_Data");

	{
		nlohmann::json& j = _CIniFileManager->getIniData()["General"];
		if (j.contains("LightData")) {
			lightData.LoadData_Internal(j["LightData"]);
		} else {

			lightData.lightColor = glm::vec4(1);
			lightData.lightPos = glm::vec4(0, 4, 5, 1);
		}
	}
	lightDataBlock = _CGraphics->CreateBuffer(&lightData, sizeof(LightDataStruct));
	lightDataBlock->SetDebugObjName("Light Data Buffer");
	_CManager->RegisterShaderUniform(lightDataBlock, "Light_Data");

	colorTest.Color = glm::vec4(1, 0, 1, 1);
	testUniform2 = _CGraphics->CreateBuffer(&colorTest, sizeof(TestUniformStruct2));
	testUniform2->SetDebugObjName("Color Test Buffer");
	_CManager->RegisterShaderUniform(testUniform2, "Shader_Data");


	//SCENE
	scene.m_FilePath = "Scenes/AssetTest.scene";
	scene.m_Camera = &mainCamera;
	scene.Load();

	_CManager->m_CurrentScene = &scene;

	//todo remove this
	_CManager->tempPVMUniform = testUniform;


	mainCamera.SetFov(fov);
	mainCamera.SetAspectRatio(_CGraphics->m_Window->GetAspect());

	if (!_CArguments->IsArgument("DontUseIniCamera")) {
		nlohmann::json& j = _CIniFileManager->getIniData()["General"];
		if (j.contains("Camera")) {
			mainCamera.LoadData_Internal(j["Camera"]);
		}
	}

	fov = mainCamera.GetFov();

	//SCENE TEST
	if (false) {


		ObjectDrawTest* tree = (ObjectDrawTest*)scene.GetObjectByName("tree");
		if (tree == nullptr) {
			tree = new ObjectDrawTest();
			tree->m_Name = "tree";
			tree->m_Color = Vector3(1, 1, 1);
			tree->m_ObjectModel = testModel;
			tree->m_ModelUniform = testUniform;
			tree->m_ColorUniform = testUniform2;
			tree->m_Transform.SetScale(Vector3(0.05f));
			tree->m_Transform.SetPosition(glm::vec3(0, -5, 0));
			scene.AddObject(tree);
		}

		ObjectDrawTest* groundPlane = (ObjectDrawTest*)scene.GetObjectByName("ground");
		if (groundPlane == nullptr) {
			groundPlane = new ObjectDrawTest();
			groundPlane->m_Name = "ground";
			groundPlane->m_Color = Vector3(1, 1, 1);
			groundPlane->m_ObjectModel = groundModel;
			groundPlane->m_ModelUniform = testUniform;
			groundPlane->m_ColorUniform = testUniform2;
			groundPlane->m_Transform.SetScale(Vector3(0.1f));
			groundPlane->m_Transform.SetPosition(glm::vec3(-15, -5, 15));
			scene.AddObject(groundPlane);
		}

		ObjectDrawTest* pathObj = (ObjectDrawTest*)scene.GetObjectByName("pathObj");
		if (pathObj == nullptr) {
			pathObj = new ObjectDrawTest();
			pathObj->m_Name = "pathObj";
			pathObj->m_Color = Vector3(1, 1, 1);
			pathObj->m_ObjectModel = pathModel;
			pathObj->m_ModelUniform = testUniform;
			pathObj->m_ColorUniform = testUniform2;
			pathObj->m_Transform.SetScale(Vector3(0.1f));
			pathObj->m_Transform.SetPosition(glm::vec3(-15, -5, -15));
			scene.AddObject(pathObj);
		}

	}

	testShader->FindUnlinkedUniforms();

	//Simple Shader.shader
	{
		Shader* shader = _CShaderManager->GetShader("Simple Shader.shader");
		if (shader) {
			shader->FindUnlinkedUniforms();
		}
	}
	SpecShader->FindUnlinkedUniforms();
	BumpShader->FindUnlinkedUniforms();

	scene.Start();
}

void Game1::Update() {
	mainCamera.m_MouseMovement = _CInput->IsMouseKeyDown(IMOUSEKEY_RIGHTBUTTON);
	mainCamera.UpdateInput();


	//UPDATE OUR SCENE
	scene.Update();
}

void Game1::ImguiDraw() {
	if (ImGui::DragFloat("FOV", &fov, 1, 0, 180)) {
		mainCamera.SetFov(fov);
	}
	ImGui::DragFloat("Camera Speed", &mainCamera.MovementSpeed, 0.1f);

	{
		bool lightingChange = false;
		lightingChange |= ImGui::DragFloat3("Light Pos", &lightData.lightPos.x, 0.1f);
		lightingChange |= ImGui::ColorEdit4("Light Color", &lightData.lightColor.x, 0.1f);
		if (lightingChange) {
			lightDataBlock->UpdateBuffer(&lightData);
		}
	}
}

void Game1::Draw() {
	commonPerFrameData.time = _CTimeManager->m_CurrentTime;
	commonPerFrameData.screenSize = glm::vec2(_CGraphics->m_Window->m_WindowWidth, _CGraphics->m_Window->m_WindowHeight);
	commonDataBlock->UpdateBuffer(&commonPerFrameData);



	cameraUniformData.MatrixPV = mainCamera.GetPV();
	cameraUniformData.pos = mainCamera.m_Position;
	cameraUniformBlock->UpdateBuffer(&cameraUniformData);



	//switch (ShaderVisualizer) {
	//	default:
	//	case 0:
			normalShader->Use();
	//		break;
	//	case 1:
	//		SpecShader->Use();
	//		break;
	//	case 2:
	//		BumpShader->Use();
	//		break;
	//}

	scene.Draw();
}

void Game1::Finish() {
	scene.Save();

	{//save camera to ini
		nlohmann::json& j = _CIniFileManager->getIniData()["General"]["Camera"];
		mainCamera.SaveData_Internal(j);

		//lightData.SaveData_Internal(_CIniFileManager->getIniData()["General"]["LightData"]);
		_CIniFileManager->getIniData()["General"]["LightData"] = lightData;
	}

	delete testUniform;
	delete testUniform2;
	delete commonDataBlock;
	//delete squareModel;
	//delete testTexture;
	//delete testShader;
	delete testModel;
	delete groundModel;
	delete pathModel;

}
