#include "SceneText2.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"

#define ROT_LIMIT 45.f;
#define SCALE_LIMIT 5.f;
#define LSPEED 10.f

extern ObjectManager ObjectList;

SceneText2::SceneText2()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
}

SceneText2::~SceneText2()
{
}

void SceneText2::Init()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	/*Sound Engine*/

	//irrklang::ISound* playStart = SoundEngine->play2D("Sounds//pistol.mp3", false);
	//irrklang::ISound* playBG = SoundEngine->play2D("Sounds//ambient.mp3", true);
	//irrklang::ISound* playWind = SoundEngine->play2D("Sounds//wind.mp3", true);
	//irrklang::ISound* playCar = SoundEngine->play2D("Sounds//engine.mp3", true);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Format Goes like this
	// ObjectList.[ObjectName].init(translationX, translationY, translationZ, scaleX, scaleY, scaleZ, rotationAmount, rotationAxisX, rotationAxisY, rotationAxisZ);
	// Please always leave your scaling as 1, 1, 1 unless of course you're doing something that's has nothing to do with collision, because this WILL mess up collision detection.
	// *Only use different scaling if you're doing something like an onscreen GUI or meshes.*

	ObjectList.Character.init(0, 0, 0, 1, 1, 1, 0, 0, 1, 0); // Initializing an object using Wen Xi's Object Class

	// For example you want to move 1 on the x-axis for your object.
	//ObjectList.Character.setTranslationX(ObjectList.Character.getTranslationX() + 1);

	camera[0].Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0), 0);
	camera[1].Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0), 1);
	
	screen = 0;

	Mtx44 projection;
	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);
	
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	//m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Texture.fragmentshader"); 

	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	//Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");


	glUseProgram(m_programID);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	light[0].type = Light::LIGHT_POINT;
	light[0].position.Set(0, 5, 0);
	light[0].color.Set(0.5f, 0.5f, 0.5f);
	light[0].power = 3;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &light[0].spotDirection.x);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);
	glUniform1i(m_parameters[U_NUMLIGHTS], 1); 

	meshList[GEO_RACELEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RACELEFT]->textureID = LoadTGA("Image//space3.tga");
	meshList[GEO_RACELEFT]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RACELEFT]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_RACELEFT]->material.kSpecular.Set(2.f, 2.f, 2.f);
	meshList[GEO_RACELEFT]->material.kShininess = 1.f;

	meshList[GEO_RACERIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RACERIGHT]->textureID = LoadTGA("Image//space3.tga");
	meshList[GEO_RACERIGHT]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RACERIGHT]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_RACERIGHT]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_RACERIGHT]->material.kShininess = 1.f;

	meshList[GEO_RACETOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RACETOP]->textureID = LoadTGA("Image//space3.tga");
	meshList[GEO_RACETOP]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RACETOP]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_RACETOP]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_RACETOP]->material.kShininess = 1.f;

	meshList[GEO_RACEBOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RACEBOTTOM]->textureID = LoadTGA("Image//space3.tga");
	meshList[GEO_RACEBOTTOM]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RACEBOTTOM]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_RACEBOTTOM]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_RACEBOTTOM]->material.kShininess = 1.f;

	meshList[GEO_RACEFRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RACEFRONT]->textureID = LoadTGA("Image//space3.tga");
	meshList[GEO_RACEFRONT]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RACEFRONT]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_RACEFRONT]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_RACEFRONT]->material.kShininess = 1.f;

	meshList[GEO_RACEBACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f, 1.f);
	meshList[GEO_RACEBACK]->textureID = LoadTGA("Image//space3.tga");
	meshList[GEO_RACEBACK]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RACEBACK]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_RACEBACK]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_RACEBACK]->material.kShininess = 1.f;

	meshList[GEO_STRACK1] = MeshBuilder::GenerateOBJ("left", "OBJ//straighttrack.obj");
	meshList[GEO_STRACK1]->textureID = LoadTGA("Image//roadtexture.tga");
	meshList[GEO_STRACK1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_STRACK1]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_STRACK1]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_STRACK1]->material.kShininess = 1.f;

	meshList[GEO_STRACK2] = MeshBuilder::GenerateOBJ("left", "OBJ//straighttrack.obj");
	meshList[GEO_STRACK2]->textureID = LoadTGA("Image//roadtexture.tga");
	meshList[GEO_STRACK2]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_STRACK2]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_STRACK2]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_STRACK2]->material.kShininess = 1.f;

	meshList[GEO_STRACK3] = MeshBuilder::GenerateOBJ("left", "OBJ//sidetrack.obj");
	meshList[GEO_STRACK3]->textureID = LoadTGA("Image//roadtexture.tga");
	meshList[GEO_STRACK3]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_STRACK3]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_STRACK3]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_STRACK3]->material.kShininess = 1.f;

	meshList[GEO_STRACK4] = MeshBuilder::GenerateOBJ("left", "OBJ//sidetrack.obj");
	meshList[GEO_STRACK4]->textureID = LoadTGA("Image//roadtexture.tga");
	meshList[GEO_STRACK4]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_STRACK4]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_STRACK4]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_STRACK4]->material.kShininess = 1.f;

	meshList[GEO_CTRACK1] = MeshBuilder::GenerateOBJ("left", "OBJ//curvetrack0.obj");
	meshList[GEO_CTRACK1]->textureID = LoadTGA("Image//roadtexture.tga");
	meshList[GEO_CTRACK1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CTRACK1]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_CTRACK1]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_CTRACK1]->material.kShininess = 1.f;

	meshList[GEO_CTRACK2] = MeshBuilder::GenerateOBJ("left", "OBJ//curvetrack90.obj");
	meshList[GEO_CTRACK2]->textureID = LoadTGA("Image//roadtexture.tga");
	meshList[GEO_CTRACK2]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CTRACK2]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_CTRACK2]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_CTRACK2]->material.kShininess = 1.f;

	meshList[GEO_CTRACK3] = MeshBuilder::GenerateOBJ("left", "OBJ//curvetrack180.obj");
	meshList[GEO_CTRACK3]->textureID = LoadTGA("Image//roadtexture.tga");
	meshList[GEO_CTRACK3]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CTRACK3]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_CTRACK3]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_CTRACK3]->material.kShininess = 1.f;

	meshList[GEO_CTRACK4] = MeshBuilder::GenerateOBJ("left", "OBJ//curvetrack270.obj");
	meshList[GEO_CTRACK4]->textureID = LoadTGA("Image//roadtexture.tga");
	meshList[GEO_CTRACK4]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CTRACK4]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_CTRACK4]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_CTRACK4]->material.kShininess = 1.f;

	meshList[GEO_CAR1] = MeshBuilder::GenerateOBJ("car", "OBJ//newcar.obj");
	//meshList[GEO_CAR1]->textureID = LoadTGA("Image//180981D_JeromeYeo_A2_UvTextureLayout.tga");
	//meshList[GEO_CAR1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	//meshList[GEO_CAR1]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	//meshList[GEO_CAR1]->material.kSpecular.Set(1.f, 1.f, 1.f);
	//meshList[GEO_CAR1]->material.kShininess = 1.f;
	
	meshList[GEO_CAR2] = MeshBuilder::GenerateOBJ("car2", "OBJ//newcar2.obj");
	meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture.tga");
	meshList[GEO_CAR2]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_CAR2]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_CAR2]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_CAR2]->material.kShininess = 1.f;

	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 1.f);

	//meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	//meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	currentTime = 0;
	

	elapsedTime = 0;
	bounceTime = 0;

	// for the indicator
	selection = 1;
	movedown = false;
	moveup = false;
	optionselected[0] = false;
	optionselected[1] = false;
	optionselected[2] = false;
	buy = false;


	// This was used as a test, if you want to set the default amount each player has, u can do it in the player class constructor
	player1.setBalance(1000.0f);

	// Change the Price of the car here, if you want to set the name of the car, use the same format as price
	cars[0].setPrice(500.0f);
	cars[1].setPrice(750.0f);
	cars[2].setPrice(1000.0f);
	cars[3].setPrice(100.0f);
}

void SceneText2::Update(double dt)
{
	currentTime += dt;
	isAcceleratingA = false;
	isAcceleratingB = false;
	isDeceleratingA = false;
	isDeceleratingB = false;

	keyPressed = false;
	abletoPress = true;
	elapsedTime += dt;

	if (bounceTime > elapsedTime)
	{
		abletoPress = false;

		return;
	}

	if (Application::IsKeyPressed(0x31))
	{
		glDisable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed(0x32))
	{
		glEnable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed(0x33))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (Application::IsKeyPressed(0x34))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('P'))
		light[0].position.y += (float)(LSPEED * dt);

	if (Application::IsKeyPressed('Q'))
	{
		//to do: switch light type to POINT and pass the information to
		light[0].type = Light::LIGHT_POINT;
	}
	else if (Application::IsKeyPressed('T'))
	{
		//to do: switch light type to DIRECTIONAL and pass the
		light[0].type = Light::LIGHT_DIRECTIONAL;
	}
	else if (Application::IsKeyPressed('E'))
	{
		//to do: switch light type to SPOT and pass the information to
		light[0].type = Light::LIGHT_SPOT;
	}
	//reset key
	if (Application::IsKeyPressed('R'))
	{
		accelerationA = 0;
		accelerationB = 0;
	}
	//for sudo acceleration==================================================================================================
	if (Application::IsKeyPressed('W')) {
		isAcceleratingA = true;
	}
	if (Application::IsKeyPressed('S')) {
		isDeceleratingA = true;
		//isAcceleratingB = true;
	}
	if (Application::IsKeyPressed('A')) {

	}
	if (Application::IsKeyPressed('D')) {
		
	}
	currentTime += dt;
	//getting offset from origin 
	objectA.SetisAccelerating(isAcceleratingA);
	objectA.SetisDecelerating(isDeceleratingA);
	accelerationA = objectA.returnAcceleration(dt, currentTime, 4.f) + objectA.returnDeceleration(dt, currentTime, 2.f);

	objectB.SetisAccelerating(isAcceleratingB);
	accelerationB = objectB.returnAcceleration(dt, currentTime, 1.f);;
	accelerationB = objectB.returnAcceleration(dt, currentTime, 4.f) + objectB.returnDeceleration(dt, currentTime, 2.f);
	//=========================================================================================================================
	if (Application::IsKeyPressed('W'))
	{
		playerPos.z -= (float)(LSPEED * dt);
		camera[screen].position.z -= (float)(LSPEED * dt);
	}
	if (Application::IsKeyPressed('S'))
	{
		playerPos.z += (float)(LSPEED * dt);
		camera[screen].position.z += (float)(LSPEED * dt);
	}
	if (Application::IsKeyPressed('D'))
	{
		playerPos.x += (float)(LSPEED * dt);
		camera[screen].position.x += (float)(LSPEED * dt);
	}
	if (Application::IsKeyPressed('A'))
	{
		playerPos.x -= (float)(LSPEED * dt);
		camera[screen].position.x -= (float)(LSPEED * dt);
	}

	// For Shop Interface

	if (Application::IsKeyPressed(VK_RETURN))
	{
		keyPressed = true;
		buy = true;

		if (selection == 1)
			optionselected[0] = true;
		else
			optionselected[0] = false;

		if (selection == 2)
			optionselected[1] = true;
		else
			optionselected[1] = false;

		if (selection == 3)
			optionselected[2] = true;
		else
			optionselected[2] = false;
	}

	if (Application::IsKeyPressed(VK_DOWN) && selection != 3)
	{
		selection += 1;
		keyPressed = true;
		movedown = true;
	}
	if (Application::IsKeyPressed(VK_UP) && selection != 1)
	{
		selection -= 1;
		keyPressed = true;
		moveup = true;
	}

	if (keyPressed)
	{
		bounceTime = elapsedTime + 0.1;
	}


	//camera[screen].Init(camera[screen].position , playerPos, Vector3(0, 1, 0), 0);  // option 2 for 3rd person cam

	//camera.MouseControl();
	camera[screen].Update(dt);
	CalculateFrameRate();
}

bool SceneText2::skyboxcheck()
{
	return true;
}

void SceneText2::Render()
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(camera[screen].position.x, camera[screen].position.y, camera[screen].position.z, camera[screen].target.x, camera[screen].target.y, camera[screen].target.z, camera[screen].up.x, camera[screen].up.y, camera[screen].up.z);
	modelStack.LoadIdentity();

	// passing the light direction if it is a direction light	
	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	// if it is spot light, pass in position and direction 
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		// default is point light (only position since point light is 360 degrees)
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	RenderSkybox();

	Vector3 lightPos;
	lightPos.x = light[0].position.x;
	lightPos.y = light[0].position.y;
	lightPos.z = light[0].position.z;

	modelStack.PushMatrix();
	modelStack.Translate(lightPos.x, lightPos.y, lightPos.z);
	RenderMesh(meshList[GEO_LIGHTSPHERE], false);
	modelStack.PopMatrix();

	//modelStack.PushMatrix();
	////modelStack.Rotate(rotationAngle, 0, 1, 0);
	//modelStack.Translate(accelerationA * cos(rotationAngle), -3, accelerationA * sin(rotationAngle));
	//RenderMesh(meshList[GEO_DICE], true);
	//modelStack.PopMatrix();

	/*modelStack.PushMatrix();
	modelStack.Translate(accelerationB, -3, 0);
	RenderMesh(meshList[GEO_DICE], true);*/

	/*modelStack.Translate(playerPos.x, -3 + playerPos.y, playerPos.z);
	RenderMesh(meshList[GEO_DICE], false);
	modelStack.PopMatrix();*/

	RenderObject(meshList[GEO_DICE], ObjectList.Character, true);

	//modelStack.PushMatrix();
	////scale, translate, rotate
	//modelStack.Translate(0, -3, 0);
	//RenderMesh(meshList[GEO_DICE], true);
	//modelStack.PopMatrix();

	//modelStack.PushMatrix();
	//scale, translate, rotate
	//RenderText(meshList[GEO_TEXT], "HELLO WORLD", Color(0, 1, 0));
	//modelStack.PopMatrix();

	//No transform needed
	//RenderTextOnScreen(meshList[GEO_TEXT], "Hello World", Color(0, 1, 0), 2, 0, 0);

	//=====================Shop Interface============================================

	RenderTextOnScreen(meshList[GEO_TEXT], "P1 Balance:", Color(0, 1, 0), 2, 0, 28);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(player1.getBalance()), Color(0, 1, 0), 1.75, 13, 32);
	RenderTextOnScreen(meshList[GEO_TEXT], "P2 Balance:", Color(0, 1, 0), 2, 0, 27);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(player2.getBalance()), Color(0, 1, 0), 1.75, 13, 30.8);


	// scuffed distance check sorry, just insert the distance check here
	if (camera[0].position.z < 8)
	{
		inrange = true;
		ShopUI(0); // the parameter is for the numbering of the cars, using it to print out the UI and store data
	}
	if (camera[0].position.z > 10)
	{
		inrange = true;
		ShopUI(1); 
	}

	std::string y = std::to_string(camera.position.y);
	RenderTextOnScreen(meshList[GEO_TEXT], y, Color(0, 1, 0), 3, 0, 19);
	std::string x = std::to_string(camera.position.x);
	RenderTextOnScreen(meshList[GEO_TEXT], x, Color(0, 1, 0), 3, 0, 16);
	std::string z = std::to_string(camera.position.z);
	RenderTextOnScreen(meshList[GEO_TEXT], z, Color(0, 1, 0), 3, 0, 13);

	RenderTextOnScreen(meshList[GEO_TEXT], "Hello World", Color(0, 1, 0), 2, 0, 0);

}

void SceneText2::Exit()
{
	// Cleanup here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);

}

void SceneText2::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);


	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if(mesh->textureID > 0){ 
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);} 
	else { 
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	} 
	mesh->Render(); //this line should only be called once in the whole function

	if(mesh->textureID > 0) glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneText2::RenderObject(Mesh* mesh, Object meshObject, bool enableLight)
{
	modelStack.PushMatrix();
	modelStack.Translate(meshObject.getTranslationX(), meshObject.getTranslationY(), meshObject.getTranslationZ());
	modelStack.Scale(meshObject.getScaleX(), meshObject.getScaleY(), meshObject.getScaleZ());
	modelStack.Rotate(meshObject.getRotationAmount(), meshObject.getRotateX(), meshObject.getRotateY(), meshObject.getRotateZ());
	RenderMesh(mesh, enableLight);
	modelStack.PopMatrix();
}

void SceneText2::RenderSkybox()
{
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(-50.f, 0.f, 0.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(50.f, 0.f, 0.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 50.f, 0.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
		modelStack.PushMatrix();
			modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
			RenderMesh(meshList[GEO_TOP], false);
		modelStack.PopMatrix();
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, -50.f, 0.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
		modelStack.PushMatrix();
		modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
		RenderMesh(meshList[GEO_BOTTOM], false);
		modelStack.PopMatrix();
		modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 0.f, -50.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 0.f, 50.f);
		modelStack.Scale(100.f, 100.f, 100.f);
		modelStack.Rotate(180.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();
}

void SceneText2::VerticeUpdate(Mesh* mesh, Object meshObject)
{
	if (meshObject.getLastTranslationX() != meshObject.getTranslationX() || meshObject.getLastTranslationY() != meshObject.getTranslationY() || meshObject.getLastTranslationZ() != meshObject.getTranslationZ()) // Updates the Bounding Box 8-vertices.
	{
		for (int counter = 0; counter <= 7; counter++)
		{
			mesh->vertices[counter].x = mesh->vertices[counter].x - meshObject.getLastTranslationX() + meshObject.getTranslationX();
			mesh->vertices[counter].y = mesh->vertices[counter].y - meshObject.getLastTranslationY() + meshObject.getTranslationY();
			mesh->vertices[counter].z = mesh->vertices[counter].z - meshObject.getLastTranslationZ() + meshObject.getTranslationZ();
		}

		mesh->maxX = mesh->vertices[6].x, mesh->maxY = mesh->vertices[6].y, mesh->maxZ = mesh->vertices[6].z, mesh->minX = mesh->vertices[0].x, mesh->minY = mesh->vertices[0].y, mesh->minZ = mesh->vertices[0].z;
		meshObject.setLastTranslationX(meshObject.getTranslationX());
		meshObject.setLastTranslationY(meshObject.getTranslationY());
		meshObject.setLastTranslationZ(meshObject.getTranslationZ());
	}
	if (mesh->textureID > 0) {
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else {
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render(); //this line should only be called once in the whole function

	if (mesh->textureID > 0) glBindTexture(GL_TEXTURE_2D, 0);
}

//void SceneText2::RenderSkybox()
//{
//	modelStack.PushMatrix();
//	///scale, translate, rotate 
//	modelStack.Translate(-150.f, 100.f, 0.f);
//	modelStack.Scale(301.f, 301.f, 301.f);
//	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
//	RenderMesh(meshList[GEO_RACELEFT], true);
//	modelStack.PopMatrix();
//	modelStack.PushMatrix();
//	///scale, translate, rotate 
//	modelStack.Translate(150.f, 100.f, 0.f);
//	modelStack.Scale(301.f, 301.f, 301.f);
//	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
//	RenderMesh(meshList[GEO_RACERIGHT], true);
//	modelStack.PopMatrix();
//	modelStack.PushMatrix();
//	///scale, translate, rotate 
//	modelStack.Translate(0.f, 250.f, 0.f);
//	modelStack.Scale(301.f, 301.f, 301.f);
//	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
//	modelStack.PushMatrix();
//	modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
//	RenderMesh(meshList[GEO_RACETOP], true);
//	modelStack.PopMatrix();
//	modelStack.PopMatrix();
//	modelStack.PushMatrix();
//	///scale, translate, rotate 
//	modelStack.Translate(0.f, 0.f, 0.f);
//	modelStack.Scale(301.f, 301.f, 301.f);
//	modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
//	modelStack.PushMatrix();
//	modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
//	RenderMesh(meshList[GEO_RACEBOTTOM], true);
//	modelStack.PopMatrix();
//	modelStack.PopMatrix();
//	modelStack.PushMatrix();
//	///scale, translate, rotate 
//	modelStack.Translate(0.f, 100.f, -150.f);
//	modelStack.Scale(301.f, 301.f, 301.f);
//	RenderMesh(meshList[GEO_RACEFRONT], true);
//	modelStack.PopMatrix();
//	modelStack.PushMatrix();
//	///scale, translate, rotate 
//	modelStack.Translate(0.f, 100.f, 150.f);
//	modelStack.Scale(301.f, 301.f, 301.f);
//	modelStack.Rotate(180.f, 0.f, 1.f, 0.f);
//	RenderMesh(meshList[GEO_RACEBACK], true);
//	modelStack.PopMatrix();
//
//	//modelStack.PushMatrix();
//	//modelStack.Translate(30.f, 10.f, 0.f);
//	//RenderMesh(meshList[GEO_CAR1], false);
//	//modelStack.PopMatrix();
//
//	modelStack.PushMatrix();
//	modelStack.Translate(0.f, 10.f, 0.f);
//	RenderMesh(meshList[GEO_CAR2], false);
//	modelStack.PopMatrix();
//
//	/*
//	modelStack.PushMatrix();
//	modelStack.Translate(0.f, 5.f, 0.f);
//	RenderMesh(meshList[GEO_STRACK1], false);
//	modelStack.PopMatrix();
//	*/
//
//	//RenderMesh(meshList[GEO_DICE], false);
//}

void SceneText2::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);

}

void SceneText2::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	glDisable(GL_DEPTH_TEST);

	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Scale(size, size, size);
	modelStack.Translate(x, y, 0);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void SceneText2::CalculateFrameRate()
{
	static float framesPerSecond = 0.0f;
	static int fps;
	static float lastTime = 0.0f;
	float currentTime = GetTickCount() * 0.001f;
	++framesPerSecond;
	printf("Current Frames Per Second: %d\n\n", fps);
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		fps = (int)framesPerSecond;
		framesPerSecond = 0;
	}
}

void SceneText2::RenderLeftScreen()
{
	screen = 0;
	SceneText2::Render();
}

void SceneText2::RenderRightScreen()
{
	screen = 1;
	SceneText2::Render();
}

int switchscreen2 = 0;

int SceneText2::SwitchScene()
{
	if (Application::IsKeyPressed('P'))
		switchscreen2 = 1;
	else if (Application::IsKeyPressed('O'))
		switchscreen2 = 0;

	return switchscreen2;
}

void SceneText2::ShopUI(int carnum)
{
	// Use this to display prices & names of cars later on
	modelStack.PushMatrix();
	RenderText(meshList[GEO_TEXT], std::to_string(cars[carnum].getPrice()), Color(0, 1, 0));
	modelStack.PopMatrix();

	if (player1.playerCar[carnum].getUnlocked() == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Select Car for Player 1", Color(0, 1, 0), 2, 0, 3);
	}
	else
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Purchase Car as Player 1", Color(0, 1, 0), 2, 0, 3);
	}
	if (player2.playerCar[carnum].getUnlocked() == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Select Car for Player 2", Color(0, 1, 0), 2, 0, 2);
	}
	else
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Purchase Car as Player 2", Color(0, 1, 0), 2, 0, 2);
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "Text Drive", Color(0, 1, 0), 2, 0, 1);

	printIndicator();

	if (inrange == true && keyPressed == true && player1.getBalance() >= cars[carnum].getPrice() && player1.playerCar[carnum].getUnlocked() == false && abletoPress == true && buy == true && optionselected[0] == true)
	{

		player1.setBalance(player1.getBalance() - cars[carnum].getPrice());
		player1.playerCar[carnum].setUnlocked(true);
		abletoPress = false;


	}
	if (inrange == true && keyPressed == true && player2.getBalance() >= cars[carnum].getPrice() && player2.playerCar[carnum].getUnlocked() == false && abletoPress == true && buy == true && optionselected[1] == true)
	{
		player2.setBalance(player2.getBalance() - cars[carnum].getPrice());
		player2.playerCar[carnum].setUnlocked(true);
		abletoPress = false;
	}
	if (inrange == true && keyPressed == true && abletoPress == true && optionselected[2] == true)
	{

	}
	if (inrange == true && player1.getBalance() < cars[carnum].getPrice() && optionselected[0] == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "You do not have enough money", Color(0, 1, 0), 2, 0, 26);
	}
	if (inrange == true && player2.getBalance() < cars[carnum].getPrice() && optionselected[1] == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "You do not have enough money", Color(0, 1, 0), 2, 0, 26);
	}

	inrange = false;
	abletoPress = true;
	buy = false;

}

void SceneText2::printIndicator()
{

	if (selection == 1)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(0, 1, 0), 2, 24, 3);
	}
	else if (selection == 2)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(0, 1, 0), 2, 24, 2);
	}
	else if (selection == 3)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(0, 1, 0), 2, 10, 1);
	}
}