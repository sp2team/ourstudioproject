#include "SceneRace.h"
#include "GL\glew.h"
#include "Application.h"
#include <Mtx44.h>
#include "shader.hpp"
#include "MeshBuilder.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <chrono>

#define ROT_LIMIT 45.f;
#define SCALE_LIMIT 5.f;
#define LSPEED 10.f

extern ObjectManager ObjectList;
extern ReplayRace replay[2];

SceneRace::SceneRace()
{
}

SceneRace::~SceneRace()
{
}

void SceneRace::Init()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	/*Sound Engine*/
	SoundEngine = irrklang::createIrrKlangDevice();
	SoundEngine->setSoundVolume(0.4f);

	ambience = SoundEngine->addSoundSourceFromFile("Sounds//ambient.mp3");
	wind = SoundEngine->addSoundSourceFromFile("Sounds//wind.mp3");
	wind->setDefaultVolume(0.5f);
	car = SoundEngine->addSoundSourceFromFile("Sounds//engine.mp3");
	car->setDefaultMinDistance(10);

	playStart = SoundEngine->play2D("Sounds//pistol.mp3", false, true, true);
	playBG = SoundEngine->play2D(ambience, true, true, true);
	playWind = SoundEngine->play2D(wind, true, true, true);
	playCar = SoundEngine->play3D(car, irrklang::vec3df(0, 0, 0), true, true, true);
	playCarTwo = SoundEngine->play3D(car, irrklang::vec3df(0, 0, 0), true, true, true);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ObjectList.Character.init(0, 0, 0, 1, 1, 1, 0, 0, 1, 0); // Initializing an object using Wen Xi's Object Class
	ObjectList.Character2.init(-4, 0, 0, 1, 1, 1, 0, 0, 1, 0);

	ObjectList.Car1.init(-8, 0.1, 230, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.Car2.init(8, 0, 230, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.Car3.init(5, 0.1, 0, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.Car4.init(10, 0.1, 0, 1, 1, 1, 0, 0, 1, 0);

	ObjectList.obstacle1.init(5, 0, -180, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.obstacle2.init(-5, 0, -100, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.obstacle3.init(2, 0, 80, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.obstacle4.init(0, 0, 170, 1, 1, 1, 0, 0, 1, 0);

	ObjectList.boundary.init(0, 0, 0, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.Racetrack.init(0, 0, 0, 1, 1, 1, 0, 0, 1, 0);

	// For example you want to move 1 on the x-axis for your object.
	//ObjectList.Character.setTranslationX(ObjectList.Character.getTranslationX() + 1);

	camera[0].Init(Vector3(0, 5, 10), Vector3(0, 0, 0), Vector3(0, 1, 0), 1);
	camera[1].Init(Vector3(0, 5, 10), Vector3(0, 0, 0), Vector3(0, 1, 0), 1);

	screen = 0;
	time = 0;
	playerOneYaw = 0;
	playerTwoYaw = 0;
	forwardDirection.Set(0, 0, 0);
	forwardTwoDirection.Set(0, 0, 0);
	backwardDirection.Set(0, 0, -0.05);


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
	light[0].position.Set(0, 0, 0);
	light[0].color.Set(0.f, 0.f, 0.f);
	light[0].power = 1;
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

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f, 1.f, ObjectList.leftwall);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f, 1.f, ObjectList.rightwall);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f, 1.f, ObjectList.roof);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f, 1.f, ObjectList.floor);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f, 1.f, ObjectList.frontwall);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f, 1.f, ObjectList.backwall);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_CAR1] = MeshBuilder::GenerateOBJ("dice", "OBJ//racecar1.obj", ObjectList.Car1);
	meshList[GEO_CAR1]->textureID = LoadTGA("Image//newcar.tga");

	meshList[GEO_CAR2] = MeshBuilder::GenerateOBJ("dice2", "OBJ//racecar2.obj", ObjectList.Car2);
	meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture.tga");

	meshList[GEO_CAR3] = MeshBuilder::GenerateOBJ("dice", "OBJ//racecar3.obj", ObjectList.Car3);
	meshList[GEO_CAR3]->textureID = LoadTGA("Image//newcar3.tga");

	meshList[GEO_CAR4] = MeshBuilder::GenerateOBJ("dice", "OBJ//racecar4.obj", ObjectList.Car4);
	meshList[GEO_CAR4]->textureID = LoadTGA("Image//newcar.tga");

	
	meshList[GEO_OBSTACLE1] = MeshBuilder::GenerateOBJ("dice", "OBJ//obstacle1.obj", ObjectList.obstacle1);
	meshList[GEO_OBSTACLE1]->textureID = LoadTGA("Image//newcar3.tga");

	meshList[GEO_OBSTACLE2] = MeshBuilder::GenerateOBJ("dice", "OBJ//obstacle2.obj", ObjectList.obstacle2);
	meshList[GEO_OBSTACLE2]->textureID = LoadTGA("Image//RocketColor.tga");
	
	meshList[GEO_OBSTACLE3] = MeshBuilder::GenerateOBJ("dice", "OBJ//cone.obj", ObjectList.obstacle3);
	meshList[GEO_OBSTACLE3]->textureID = LoadTGA("Image//cone.tga");

	meshList[GEO_OBSTACLE4] = MeshBuilder::GenerateOBJ("dice", "OBJ//obstacle4.obj", ObjectList.obstacle4);
	meshList[GEO_OBSTACLE4]->textureID = LoadTGA("Image//buildingext.tga");

	meshList[GEO_RACETRACK] = MeshBuilder::GenerateOBJ("dice", "OBJ//finaltrack.obj", ObjectList.Racetrack);
	meshList[GEO_RACETRACK]->textureID = LoadTGA("Image/roadtexture.tga");

	/*meshList[GEO_BOUNDARY] = MeshBuilder::GenerateOBJ("dice", "OBJ//boundary.obj", 0, 0, 0);
	meshList[GEO_BOUNDARY]->textureID = LoadTGA("Image/boundary.tga");*/

	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 3.f, ObjectList.LightSphere);

	meshList[GEO_LIGHTSPHERE2] = MeshBuilder::GenerateSphere("lightBall", Color(0.f, 1.f, 0.f), 9, 36, 3.f, ObjectList.LightSphere2);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

}

void SceneRace::Update(double dt)
{
	Vector3 cameraPos(0, 15, 20);
	Vector3 PlayerCam = (0, 0, 0);
	playerPos.Set(ObjectList.Car1.getTranslationX(), ObjectList.Car1.getTranslationY(), ObjectList.Car1.getTranslationZ());
	playerTwoPos.Set(ObjectList.Car2.getTranslationX(), ObjectList.Car2.getTranslationY(), ObjectList.Car2.getTranslationZ());
	float addition = (float)(LSPEED * dt);

	//Player 1 movement
	if (Application::IsKeyPressed('W'))
	{
		Mtx44 rotation;
		rotation.SetToRotation(-playerOneYaw, 0, 1, 0);
		if (forwardDirection.z > -0.35)
		{
			forwardDirection.z -= 0.05 * dt;
		}
		playerPos = playerPos + rotation * forwardDirection;
	}
	else
	{
		Mtx44 rotation;
		rotation.SetToRotation(-playerOneYaw, 0, 1, 0);
		if (forwardDirection.z < 0)
		{
			forwardDirection.z += 0.1 * dt;
			if (forwardDirection.z >= 0)
			{
				forwardDirection.z = 0;
			}
			playerPos = playerPos + rotation * forwardDirection;
		}
	}
	if (Application::IsKeyPressed('S'))
	{
		Mtx44 rotation;
		rotation.SetToRotation(-playerOneYaw, 0, 1, 0);
		playerPos = playerPos - rotation * backwardDirection;

		if (Application::IsKeyPressed('D'))
		{
			playerOneYaw++;
		}
		if (Application::IsKeyPressed('A'))
		{
			playerOneYaw--;
		}
	}
	if (Application::IsKeyPressed('D') && forwardDirection.z != 0)
	{
		playerOneYaw++;
	}
	if (Application::IsKeyPressed('A') && forwardDirection.z != 0)
	{
		playerOneYaw--;
	}

	//Player 2 movement
	if (Application::IsKeyPressed(VK_UP))
	{
		Mtx44 rotation;
		rotation.SetToRotation(-playerTwoYaw, 0, 1, 0);
		if (forwardTwoDirection.z > -0.35)
		{
			forwardTwoDirection.z -= 0.05 * dt;
		}
		playerTwoPos = playerTwoPos + rotation * forwardTwoDirection;
	}
	else
	{
		Mtx44 rotation;
		rotation.SetToRotation(-playerTwoYaw , 0, 1, 0);
		if (forwardTwoDirection.z < 0)
		{
			forwardTwoDirection.z += 0.1 * dt;
			if (forwardTwoDirection.z >= 0)
			{
				forwardTwoDirection.z = 0;
			}
			playerTwoPos = playerTwoPos + rotation * forwardTwoDirection;
		}
	}
	if (Application::IsKeyPressed(VK_DOWN))
	{
		Mtx44 rotation;
		rotation.SetToRotation(-playerTwoYaw, 0, 1, 0);
		playerTwoPos = playerTwoPos - rotation * Vector3(0, 0, -0.2f);
		if (Application::IsKeyPressed(VK_RIGHT))
		{
			playerTwoYaw++;
		}
		if (Application::IsKeyPressed(VK_LEFT))
		{
			playerTwoYaw--;
		}
	}
	if (Application::IsKeyPressed(VK_RIGHT) && forwardTwoDirection.z != 0 )
	{
		playerTwoYaw++;
	}
	if (Application::IsKeyPressed(VK_LEFT) && forwardTwoDirection.z != 0)
	{
		playerTwoYaw--;
	}


	/*camera[0].position.Set(playerPos.x + 0.01, playerPos.y + 10, playerPos.z - 10);*/
	Mtx44 rotation;
	rotation.SetToRotation(-playerOneYaw, 0, 1, 0);
	PlayerCam = (rotation * cameraPos) + playerPos;
	camera[0].Init(PlayerCam, playerPos, Vector3(0, 1, 0), 1);

	rotation.SetToRotation(-playerTwoYaw, 0, 1, 0);
	camera[1].Init(((rotation * cameraPos) + playerTwoPos), playerTwoPos, Vector3(0, 1, 0), 1);

	ObjectList.Car1.setTranslationXYZ(playerPos.x, playerPos.y, playerPos.z);
	ObjectList.Car1.setRotationAmount(-playerOneYaw);
	ObjectList.Car2.setTranslationXYZ(playerTwoPos.x, playerTwoPos.y, playerTwoPos.z);
	ObjectList.Car2.setRotationAmount(-playerTwoYaw);
	replay[0].saveFrame(ObjectList.Character);
	replay[1].saveFrame(ObjectList.Character2);

	// Play Sounds for main player
	if (forwardDirection.z < 0 && playCar->getIsPaused())
	{
		playBG = SoundEngine->play2D(ambience, true, false, true);
		playWind = SoundEngine->play2D(wind, true, false, true);
		playCar = SoundEngine->play3D(car, irrklang::vec3df(playerPos.x, playerPos.y, playerPos.z), true, false, true);
	}
	if (forwardDirection == 0 && playCar)
	{
		playBG->setIsPaused();
		playWind->setIsPaused();
		playCar->setIsPaused();
	}
	// For player 2
	if (forwardTwoDirection.z < 0 && playCarTwo->getIsPaused())
	{
		playCarTwo = SoundEngine->play3D(car, irrklang::vec3df(playerTwoPos.x, playerTwoPos.y, playerTwoPos.z), true, false, true);
	}
	if (forwardTwoDirection == 0 && playCarTwo)
	{
		playCarTwo->setIsPaused();
	}

	// Set Listening position and car position
	SoundEngine->setListenerPosition(irrklang::vec3df(PlayerCam.x, PlayerCam.y, PlayerCam.z), irrklang::vec3df(playerPos.x, playerPos.y, playerPos.z));
	playCar->setPosition(irrklang::vec3df(playerPos.x, playerPos.y, playerPos.z));
	playCarTwo->setPosition(irrklang::vec3df(playerTwoPos.x, playerTwoPos.y, playerTwoPos.z));
}

bool SceneRace::skyboxcheck()
{
	if (camera[0].position.x > 83 && camera[0].position.x < 100 && camera[0].position.z < 10 && camera[0].position.z > -10)
	{
		return true;
	}
}

void SceneRace::Render()
{
	viewStack.LoadIdentity();
	viewStack.LookAt(camera[screen].position.x, camera[screen].position.y, camera[screen].position.z, camera[screen].target.x, camera[screen].target.y, camera[screen].target.z, camera[screen].up.x, camera[screen].up.y, camera[screen].up.z);
	modelStack.LoadIdentity();

	modelStack.PushMatrix();
	modelStack.Scale(2.5f, 2.5f, 2.5f);
	RenderSkybox();
	modelStack.PopMatrix();

	/*Render the two car*/
	//RenderObject(meshList[GEO_CAR1], ObjectList.Character, true);
	//RenderObject(meshList[GEO_CAR2], ObjectList.Character2, true);

	RenderObject(meshList[GEO_CAR1], ObjectList.Car1, false);
	RenderObject(meshList[GEO_CAR2], ObjectList.Car2, false);
	/*RenderObject(meshList[GEO_CAR3], ObjectList.Car3, false);
	RenderObject(meshList[GEO_CAR4], ObjectList.Car4, false);*/

	RenderObject(meshList[GEO_RACETRACK], ObjectList.Racetrack, false);

	RenderObject(meshList[GEO_OBSTACLE1], ObjectList.obstacle1, false);
    RenderObject(meshList[GEO_OBSTACLE2], ObjectList.obstacle2, false);
	RenderObject(meshList[GEO_OBSTACLE3], ObjectList.obstacle3, false);
	RenderObject(meshList[GEO_OBSTACLE4], ObjectList.obstacle4, false);

	//RenderObject(meshList[GEO_BOUNDARY], ObjectList.boundary, false);

	//Render lap time
	RenderTextOnScreen(meshList[GEO_TEXT], "P1 Time:", Color(0, 1, 0), 2, 0, 28);
	RenderTextOnScreen(meshList[GEO_TEXT], "X:" +  std::to_string(ObjectList.Car1.getTranslationX()) + " Z:" + std::to_string(ObjectList.Car1.getTranslationZ()), Color(0, 1, 0), 2, 0, 27);
	RenderTextOnScreen(meshList[GEO_TEXT], CalculateTime(), Color(0, 1, 0), 1.75, 13, 32);
	RenderTextOnScreen(meshList[GEO_TEXT], "P2 Time:", Color(0, 1, 0), 2, 0, 28);
	RenderTextOnScreen(meshList[GEO_TEXT], CalculateTime(), Color(0, 1, 0), 1.75, 13, 32);
}

void SceneRace::Exit()
{
	// Cleanup here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	replay[0].resetReplay();
	replay[1].resetReplay();

	playBG->setIsPaused();
	playWind->setIsPaused();
	playCar->setIsPaused();
	playCarTwo->setIsPaused();
	playBG->drop();
	playWind->drop();
	playCar->drop();
	playCarTwo->drop();
	SoundEngine->drop();

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneRace::RenderMesh(Mesh* mesh, bool enableLight)
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

void SceneRace::RenderObject(Mesh* mesh, Object meshObject, bool enableLight)
{
	modelStack.PushMatrix();
	modelStack.Translate(meshObject.getTranslationX(), meshObject.getTranslationY(), meshObject.getTranslationZ());
	modelStack.Scale(meshObject.getScaleX(), meshObject.getScaleY(), meshObject.getScaleZ());
	modelStack.Rotate(meshObject.getRotationAmount(), meshObject.getRotateX(), meshObject.getRotateY(), meshObject.getRotateZ());
	RenderMesh(mesh, enableLight);
	modelStack.PopMatrix();
}

void SceneRace::RenderSkybox()
{
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(-100.f, 0.f, 0.f);
		modelStack.Scale(200.f, 200.f, 200.f);
		modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(100.f, 0.f, 0.f);
		modelStack.Scale(200.f, 200.f, 200.f);
		modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 50.f, 0.f);
		modelStack.Scale(200.f, 200.f, 200.f);
		modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
		modelStack.PushMatrix();
			modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
			RenderMesh(meshList[GEO_TOP], false);
		modelStack.PopMatrix();
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 0.f, 0.f);
		modelStack.Scale(200.f, 200.f, 200.f);
		modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
		modelStack.PushMatrix();
		modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
		RenderMesh(meshList[GEO_BOTTOM], false);
		modelStack.PopMatrix();
		modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 0.f, -100.f);
		modelStack.Scale(200.f, 200.f, 200.f);
		RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();
	modelStack.PushMatrix();
		///scale, translate, rotate 
		modelStack.Translate(0.f, 0.f, 100.f);
		modelStack.Scale(200.f, 200.f, 200.f);
		modelStack.Rotate(180.f, 0.f, 1.f, 0.f);
		RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();
}

void SceneRace::RenderText(Mesh* mesh, std::string text, Color color)
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

void SceneRace::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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

//void SceneRace::CarMovement(double dt)
//{
//	Vector3 cameraPos(0, 5, 10);
//	Vector3 PlayerCam = (0, 0, 0);
//	playerPos.Set(ObjectList.Car1.getTranslationX(), ObjectList.Car1.getTranslationY(), ObjectList.Car1.getTranslationZ());
//	playerTwoPos.Set(ObjectList.Car2.getTranslationX(), ObjectList.Car2.getTranslationY(), ObjectList.Car2.getTranslationZ());
//	float addition = (float)(LSPEED * dt);
//
//	//Player 1 movement
//	if (Application::IsKeyPressed('W'))
//	{
//		Mtx44 rotation;
//		rotation.SetToRotation(-playerOneYaw, 0, 1, 0);
//		if (forwardDirection.z > -0.35)
//		{
//			forwardDirection.z -= 0.05 * dt;
//		}
//		playerPos = playerPos + rotation * forwardDirection;
//	}
//	else
//	{
//		Mtx44 rotation;
//		rotation.SetToRotation(-playerOneYaw, 0, 1, 0);
//		if (forwardDirection.z < 0)
//		{
//			forwardDirection.z += 0.1 * dt;
//			if (forwardDirection.z >= 0)
//			{
//				forwardDirection.z = 0;
//			}
//			playerPos = playerPos + rotation * forwardDirection;
//		}
//	}
//	if (Application::IsKeyPressed('S'))
//	{
//		Mtx44 rotation;
//		rotation.SetToRotation(-playerOneYaw, 0, 1, 0);
//		playerPos = playerPos - rotation * backwardDirection;
//
//		if (Application::IsKeyPressed('D'))
//		{
//			playerOneYaw++;
//		}
//		if (Application::IsKeyPressed('A'))
//		{
//			playerOneYaw--;
//		}
//	}
//	if (Application::IsKeyPressed('D') && forwardDirection.z != 0)
//	{
//		playerOneYaw++;
//	}
//	if (Application::IsKeyPressed('A') && forwardDirection.z != 0)
//	{
//		playerOneYaw--;
//	}
//
//	//Player 2 movement
//	if (Application::IsKeyPressed(VK_UP))
//	{
//		Mtx44 rotation;
//		rotation.SetToRotation(-playerTwoYaw, 0, 1, 0);
//		if (forwardTwoDirection.z > -0.35)
//		{
//			forwardTwoDirection.z -= 0.05 * dt;
//		}
//		playerTwoPos = playerTwoPos + rotation * forwardTwoDirection;
//	}
//	else
//	{
//		Mtx44 rotation;
//		rotation.SetToRotation(-playerTwoYaw, 0, 1, 0);
//		if (forwardTwoDirection.z < 0)
//		{
//			forwardTwoDirection.z += 0.1 * dt;
//			if (forwardTwoDirection.z >= 0)
//			{
//				forwardTwoDirection.z = 0;
//			}
//			playerTwoPos = playerTwoPos + rotation * forwardTwoDirection;
//		}
//	}
//	if (Application::IsKeyPressed(VK_DOWN))
//	{
//		Mtx44 rotation;
//		rotation.SetToRotation(-playerTwoYaw, 0, 1, 0);
//		playerTwoPos = playerTwoPos - rotation * Vector3(0, 0, -0.2f);
//		if (Application::IsKeyPressed(VK_RIGHT))
//		{
//			playerTwoYaw++;
//		}
//		if (Application::IsKeyPressed(VK_LEFT))
//		{
//			playerTwoYaw--;
//		}
//	}
//	if (Application::IsKeyPressed(VK_RIGHT) && forwardTwoDirection.z != 0)
//	{
//		playerTwoYaw++;
//	}
//	if (Application::IsKeyPressed(VK_LEFT) && forwardTwoDirection.z != 0)
//	{
//		playerTwoYaw--;
//	}
//
//
//	/*camera[0].position.Set(playerPos.x + 0.01, playerPos.y + 10, playerPos.z - 10);*/
//	Mtx44 rotation;
//	rotation.SetToRotation(-playerOneYaw, 0, 1, 0);
//	PlayerCam = (rotation * cameraPos) + playerPos;
//	camera[0].Init(PlayerCam, playerPos, Vector3(0, 1, 0), 1);
//
//	rotation.SetToRotation(-playerTwoYaw, 0, 1, 0);
//	camera[1].Init(((rotation * cameraPos) + playerTwoPos), playerTwoPos, Vector3(0, 1, 0), 1);
//
//	ObjectList.Car1.setTranslationXYZ(playerPos.x, playerPos.y, playerPos.z);
//	ObjectList.Car1.setRotationAmount(-playerOneYaw);
//	ObjectList.Car2.setTranslationXYZ(playerTwoPos.x, playerTwoPos.y, playerTwoPos.z);
//	ObjectList.Car2.setRotationAmount(-playerTwoYaw);
//
//	// Play Sounds for main player
//	if (forwardDirection.z < 0 && playCar->getIsPaused())
//	{
//		playBG = SoundEngine->play2D(ambience, true, false, true);
//		playWind = SoundEngine->play2D(wind, true, false, true);
//		playCar = SoundEngine->play3D(car, irrklang::vec3df(playerPos.x, playerPos.y, playerPos.z), true, false, true);
//	}
//	if (forwardDirection == 0 && playCar)
//	{
//		playBG->setIsPaused();
//		playWind->setIsPaused();
//		playCar->setIsPaused();
//	}
//	// For player 2
//	if (forwardTwoDirection.z < 0)
//	{
//		playCarTwo = SoundEngine->play3D(car, irrklang::vec3df(playerTwoPos.x, playerTwoPos.y, playerTwoPos.z), true, false, true);
//	}
//	if (forwardTwoDirection == 0)
//	{
//		playCarTwo->setIsPaused();
//	}
//
//	// Set Listening position and car position
//	SoundEngine->setListenerPosition(irrklang::vec3df(PlayerCam.x, PlayerCam.y, PlayerCam.z), irrklang::vec3df(playerPos.x, playerPos.y, playerPos.z));
//	playCar->setPosition(irrklang::vec3df(playerPos.x, playerPos.y, playerPos.z));
//	playCarTwo->setPosition(irrklang::vec3df(playerTwoPos.x, playerTwoPos.y, playerTwoPos.z));
//}

string SceneRace::CalculateTime()
{	
	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime;
	float fElapsedTime;

	tp2 = std::chrono::system_clock::now();
	elapsedTime = tp2 - tp1;
	tp1 = tp2;
	fElapsedTime = elapsedTime.count();

	time += fElapsedTime * 10000;
	return to_string(time);
}

void SceneRace::RenderLeftScreen()
{
	screen = 0;
	SceneRace::Render();
}

void SceneRace::RenderRightScreen()
{
	screen = 1;
	SceneRace::Render();
}

bool SceneRace::SwitchScene()
{
	if (Application::IsKeyPressed('P'))
		return true;
	else
		return false;
}

void SceneRace::Reset()
{
	// Cleanup here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}

	// Cleanup Sounds
	playBG->setIsPaused();
	playWind->setIsPaused();
	playCar->setIsPaused();
	playCarTwo->setIsPaused();
	playBG->drop();
	playWind->drop();
	playCar->drop();
	playCarTwo->drop();
	SoundEngine->drop();

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);

	Init();
}