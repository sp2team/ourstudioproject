#include "SceneText.h"
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
extern ReplayRace replay[2];
Player playerData;

SceneText::SceneText()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
}

SceneText::~SceneText()
{
}

void SceneText::Init()
{
	ytranslate = 0.f;
	ytranslate2 = 0.f;
	scenecheck = false;
	rotation1 = rotation2 = 0.f;
	lightposx = -48.f;
	lightposz = -48.f;
	ringposx = 55.f;
	ringposz = 55.f;
	toggleShop = true;

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	/*Sound Engine*/
	SoundEngine = irrklang::createIrrKlangDevice();
	SoundEngine->setSoundVolume(0.3f);
	playMusic = SoundEngine->play2D("Sounds//Elevator music.mp3", true, false, true);

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

	ObjectList.NPC.init(55, 0, 4, 1, 1, 1, 0, 0, 1, 0); // Initializing an object using Wen Xi's Object Class

	ObjectList.pillar1.init(88, 0, 83, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.pillar2.init(-88, 0, 83, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.pillar3.init(88, 0, -83, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.pillar4.init(-88, 0, -83, 1, 1, 1, 0, 0, 1, 0);

	ObjectList.leftwall.init(-100, 100, 0, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.rightwall.init(100, 100, 0, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.roof.init(0, 200, 0, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.floor.init(0, 0, 0, 200, 200, 200, -90, 1, 0, 0);
	ObjectList.frontwall.init(0, 100, -100, 1, 1, 1, 0, 1, 0, 0);
	ObjectList.backwall.init(0, 100, 100, 1, 1, 1, 0, 1, 0, 0);

	ObjectList.leftwall2.init(-150, 50, 0, 300, 300, 300, 90, 0, 1, 0);
	ObjectList.rightwall2.init(150, 50, 0, 300, 300, 300, -90, 0, 1, 0);
	ObjectList.roof2.init(0, 200, 0, 300, 300, 300, 90, 1, 0, 0);
	ObjectList.frontwall2.init(0, 50, -150, 300, 300, 300, 0, 0, 1, 0);
	ObjectList.backwall2.init(0, 50, 150, 300, 300, 300, 180, 0, 1, 0);

	ObjectList.ring1.init(0, ytranslate, 91, 1, 1, 1, 0, 1, 0, 0);
	ObjectList.ring2.init(ringposx, ytranslate2, ringposz, 1, 1, 1, 0, 1, 0, 0);

	ObjectList.turntable1.init(55, 10, 55, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.turntable2.init(55, 10, -55, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.turntable3.init(-55, 10, 55, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.turntable4.init(-55, 10, -55, 1, 1, 1, 0, 0, 1, 0);

	ObjectList.newcar1.init(53.f, 10.f, 50.f, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.newcar2.init(50.f, 18.f, -50.f, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.newcar3.init(-55.f, 15.f, 55.f, 1, 1, 1, 0, 0, 1, 0);
	ObjectList.newcar4.init(-60.f, 18.f, -60.f, 1, 1, 1, 0, 0, 1, 0);

	ObjectList.fortuneWheel.init(30.f, 12.f, 95.f, 1, 1, 1, 0, 0, 0, 1);
	ObjectList.fortuneWheelIndicator.init(30.f, 12.f, 91.f, 1, 1, 1, 0, 0, 0, 1);

	// For example you want to move 1 on the x-axis for your object.
	//ObjectList.Character.setTranslationX(ObjectList.Character.getTranslationX() + 1);

	camera.Init(Vector3(0, 15, 10), Vector3(0, 10, 0), Vector3(0, 1, 0), 0);

	Mtx44 projection;
	projection.SetToPerspective(90.f , 4.f / 3.f, 0.1f, 1000.f);
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

	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

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
	light[0].color.Set(0.5f, 0.5f, 0.5f);
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	light[1].type = Light::LIGHT_POINT;
	light[1].position.Set(lightposx, 50, lightposz);
	light[1].color.Set(0.f, 0.5f, 0.f);
	light[1].power = 10;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = cos(Math::DegreeToRadian(35));
	light[1].cosInner = cos(Math::DegreeToRadian(45));
	light[1].exponent = 3.f;
	light[1].spotDirection.Set(0.f, 1.f, 0.f);

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

	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &light[1].spotDirection.x);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	meshList[GEO_LEFT] = MeshBuilder::GenerateOBJ("left", "OBJ//wallLR.obj", ObjectList.leftwall.getOffsetX(), ObjectList.leftwall .getOffsetY(), ObjectList.leftwall.getOffsetZ());
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//walltexture.tga");
	meshList[GEO_LEFT]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_LEFT]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_LEFT]->material.kSpecular.Set(2.f, 2.f, 2.f);
	meshList[GEO_LEFT]->material.kShininess = 1.f;

	meshList[GEO_RIGHT] = MeshBuilder::GenerateOBJ("right", "OBJ//wallLR.obj", ObjectList.rightwall.getOffsetX(), ObjectList.rightwall.getOffsetY(), ObjectList.rightwall.getOffsetZ());
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//walltexture.tga");
	meshList[GEO_RIGHT]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_RIGHT]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_RIGHT]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_RIGHT]->material.kShininess = 1.f;

	meshList[GEO_TOP] = MeshBuilder::GenerateOBJ("top", "OBJ//top.obj", 0, 0, 0);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//silver.tga");
	meshList[GEO_TOP]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TOP]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_TOP]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_TOP]->material.kShininess = 1.f;

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//floortexture.tga");
	meshList[GEO_BOTTOM]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_BOTTOM]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_BOTTOM]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_BOTTOM]->material.kShininess = 1.f;

	meshList[GEO_FRONT] = MeshBuilder::GenerateOBJ("front", "OBJ//wallFB.obj", ObjectList.frontwall.getOffsetX(), ObjectList.frontwall.getOffsetY(), ObjectList.frontwall.getOffsetZ());
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//walltexture.tga");
	meshList[GEO_FRONT]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_FRONT]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_FRONT]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_FRONT]->material.kShininess = 1.f;

	meshList[GEO_BACK] = MeshBuilder::GenerateOBJ("back", "OBJ//wallFB.obj", ObjectList.backwall.getOffsetX(), ObjectList.backwall.getOffsetY(), ObjectList.backwall.getOffsetZ());
	meshList[GEO_BACK]->textureID = LoadTGA("Image//walltexture.tga");
	meshList[GEO_BACK]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_BACK]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_BACK]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_BACK]->material.kShininess = 1.f;

	meshList[GEO_LEFT2] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_LEFT2]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_TEXTBG] = MeshBuilder::GenerateQuad("textbackground", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_TEXTBG]->textureID = LoadTGA("Image//textbg.tga");

	meshList[GEO_RIGHT2] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_RIGHT2]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_TOP2] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_TOP2]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_FRONT2] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_FRONT2]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_BACK2] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_BACK2]->textureID = LoadTGA("Image//space2.tga");

	meshList[GEO_CAR1] = MeshBuilder::GenerateOBJ("Dice", "OBJ//newcar.obj", ObjectList.newcar1.getOffsetX(), ObjectList.newcar1.getOffsetY(), ObjectList.newcar1.getOffsetZ());
	meshList[GEO_CAR1]->textureID = LoadTGA("Image//newcar.tga");

	meshList[GEO_CAR2] = MeshBuilder::GenerateOBJ("Dice", "OBJ//newcar2.obj", ObjectList.newcar2.getOffsetX(), ObjectList.newcar2.getOffsetY(), ObjectList.newcar2.getOffsetZ());
	meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture.tga");

	meshList[GEO_CAR3] = MeshBuilder::GenerateOBJ("Dice", "OBJ//newcar3.obj", ObjectList.newcar3.getOffsetX(), ObjectList.newcar3.getOffsetY(), ObjectList.newcar3.getOffsetZ());
	meshList[GEO_CAR3]->textureID = LoadTGA("Image//newcar3.tga");

	meshList[GEO_CAR4] = MeshBuilder::GenerateOBJ("Dice", "OBJ//newcar4.obj", ObjectList.newcar4.getOffsetX(), ObjectList.newcar4.getOffsetY(), ObjectList.newcar4.getOffsetZ());
	meshList[GEO_CAR4]->textureID = LoadTGA("Image//180981D_JeromeYeo_A2_UvTextureLayout.tga");

	meshList[GEO_TURNTABLE1] = MeshBuilder::GenerateOBJ("Dice", "OBJ//turntable.obj", ObjectList.turntable1.getOffsetX(), ObjectList.turntable1.getOffsetY(), ObjectList.turntable1.getOffsetZ());
	meshList[GEO_TURNTABLE1]->textureID = LoadTGA("Image//walltexture.tga");
	meshList[GEO_TURNTABLE1]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TURNTABLE1]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_TURNTABLE1]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_TURNTABLE1]->material.kShininess = 1.f;

	meshList[GEO_TURNTABLE2] = MeshBuilder::GenerateOBJ("Dice", "OBJ//turntable.obj", ObjectList.turntable2.getOffsetX(), ObjectList.turntable2.getOffsetY(), ObjectList.turntable2.getOffsetZ());
	meshList[GEO_TURNTABLE2]->textureID = LoadTGA("Image//walltexture.tga");
	meshList[GEO_TURNTABLE2]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TURNTABLE2]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_TURNTABLE2]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_TURNTABLE2]->material.kShininess = 1.f;

	meshList[GEO_TURNTABLE3] = MeshBuilder::GenerateOBJ("Dice", "OBJ//turntable.obj", ObjectList.turntable3.getOffsetX(), ObjectList.turntable3.getOffsetY(), ObjectList.turntable3.getOffsetZ());
	meshList[GEO_TURNTABLE3]->textureID = LoadTGA("Image//walltexture.tga");
	meshList[GEO_TURNTABLE3]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TURNTABLE3]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_TURNTABLE3]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_TURNTABLE3]->material.kShininess = 1.f;

	meshList[GEO_TURNTABLE4] = MeshBuilder::GenerateOBJ("Dice", "OBJ//turntable.obj", ObjectList.turntable4.getOffsetX(), ObjectList.turntable4.getOffsetY(), ObjectList.turntable4.getOffsetZ());
	meshList[GEO_TURNTABLE4]->textureID = LoadTGA("Image//walltexture.tga");
	meshList[GEO_TURNTABLE4]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_TURNTABLE4]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_TURNTABLE4]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_TURNTABLE4]->material.kShininess = 1.f;

	meshList[GEO_PILLAR] = MeshBuilder::GenerateOBJ("Dice", "OBJ//Pedestal.obj", ObjectList.pillar1.getOffsetX(), ObjectList.pillar1.getOffsetY(), ObjectList.pillar1.getOffsetZ());
	meshList[GEO_PILLAR]->textureID = LoadTGA("Image//pillar.tga");
	meshList[GEO_PILLAR]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PILLAR]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_PILLAR]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_PILLAR]->material.kShininess = 1.f;

	meshList[GEO_PILLAR2] = MeshBuilder::GenerateOBJ("Dice", "OBJ//Pedestal.obj", ObjectList.pillar2.getOffsetX(), ObjectList.pillar2.getOffsetY(), ObjectList.pillar2.getOffsetZ());
	meshList[GEO_PILLAR2]->textureID = LoadTGA("Image//pillar.tga");
	meshList[GEO_PILLAR2]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PILLAR2]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_PILLAR2]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_PILLAR2]->material.kShininess = 1.f;

	meshList[GEO_PILLAR3] = MeshBuilder::GenerateOBJ("Dice", "OBJ//Pedestal.obj", ObjectList.pillar3.getOffsetX(), ObjectList.pillar3.getOffsetY(), ObjectList.pillar3.getOffsetZ());
	meshList[GEO_PILLAR3]->textureID = LoadTGA("Image//pillar.tga");
	meshList[GEO_PILLAR3]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PILLAR3]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_PILLAR3]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_PILLAR3]->material.kShininess = 1.f;

	meshList[GEO_PILLAR4] = MeshBuilder::GenerateOBJ("Dice", "OBJ//Pedestal.obj", ObjectList.pillar4.getOffsetX(), ObjectList.pillar4.getOffsetY(), ObjectList.pillar4.getOffsetZ());
	meshList[GEO_PILLAR4]->textureID = LoadTGA("Image//pillar.tga");
	meshList[GEO_PILLAR4]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_PILLAR4]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_PILLAR4]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_PILLAR4]->material.kShininess = 1.f;

	meshList[GEO_NPC] = MeshBuilder::GenerateOBJ("Dice","OBJ//doorman.obj", ObjectList.NPC.getOffsetX(), ObjectList.NPC.getOffsetY(), ObjectList.NPC.getOffsetZ());
	meshList[GEO_NPC]->textureID = LoadTGA("Image//doorman.tga");

	meshList[GEO_RING] = MeshBuilder::GenerateOBJ("ring", "OBJ//ring.obj", ObjectList.ring1.getOffsetX(), ObjectList.ring1.getOffsetY(), ObjectList.ring1.getOffsetZ());
	meshList[GEO_RING]->textureID = LoadTGA("Image//greenring.tga");

	meshList[GEO_RING2] = MeshBuilder::GenerateOBJ("ring", "OBJ//ring2.obj", ObjectList.ring2.getOffsetX(), ObjectList.ring2.getOffsetY(), ObjectList.ring2.getOffsetZ());
	meshList[GEO_RING2]->textureID = LoadTGA("Image//greenring.tga");

	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 3.f, ObjectList.LightSphere.getOffsetX(), ObjectList.LightSphere.getOffsetY(), ObjectList.LightSphere.getOffsetZ());

	meshList[GEO_LIGHTSPHERE2] = MeshBuilder::GenerateSphere("lightBall", Color(0.f, 1.f, 0.f), 9, 36, 3.f, ObjectList.LightSphere2.getOffsetX(), ObjectList.LightSphere2.getOffsetY(), ObjectList.LightSphere2.getOffsetZ());

	meshList[GEO_FORTUNEWHEEL] = MeshBuilder::GenerateOBJ("Wheel Of Fortune", "OBJ//WheelOfFortune.obj", ObjectList.fortuneWheel.getOffsetX(), ObjectList.fortuneWheel.getOffsetY(), ObjectList.fortuneWheel.getOffsetZ());
	meshList[GEO_FORTUNEWHEEL]->textureID = LoadTGA("Image//texture_wheeloffortune.tga");
	meshList[GEO_FORTUNEWHEEL]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_FORTUNEWHEEL]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_FORTUNEWHEEL]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_FORTUNEWHEEL]->material.kShininess = 1.f;

	meshList[GEO_FORTUNEWHEEL_INDICATOR] = MeshBuilder::GenerateOBJ("Wheel Of Fortune", "OBJ//fortunewheelindicator.obj", 0, 0, 0);
	meshList[GEO_FORTUNEWHEEL_INDICATOR]->textureID = LoadTGA("Image//boundary.tga");
	meshList[GEO_FORTUNEWHEEL_INDICATOR]->material.kAmbient.Set(0.6f, 0.6f, 0.6f);
	meshList[GEO_FORTUNEWHEEL_INDICATOR]->material.kDiffuse.Set(0.2f, 0.2f, 0.2f);
	meshList[GEO_FORTUNEWHEEL_INDICATOR]->material.kSpecular.Set(1.f, 1.f, 1.f);
	meshList[GEO_FORTUNEWHEEL_INDICATOR]->material.kShininess = 1.f;


	meshList[GEO_TURNTABLE1_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_TURNTABLE1]->vertices);
	meshList[GEO_TURNTABLE2_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_TURNTABLE2]->vertices);
	meshList[GEO_TURNTABLE3_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_TURNTABLE3]->vertices);
	meshList[GEO_TURNTABLE4_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_TURNTABLE4]->vertices);
	meshList[GEO_PILLAR_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_PILLAR]->vertices);
	meshList[GEO_PILLAR2_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_PILLAR2]->vertices);
	meshList[GEO_PILLAR3_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_PILLAR3]->vertices);
	meshList[GEO_PILLAR4_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_PILLAR4]->vertices);
	meshList[GEO_FORTUNEWHEEL_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_FORTUNEWHEEL]->vertices);
	meshList[GEO_NPC_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_NPC]->vertices);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

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
	//for texture
	playerData.player1Texture = 1;
	playerData.player2Texture = 1;
	texture = 1;
	perviousLocation = 0;

	// This was used as a test, if you want to set the default amount each player has, u can do it in the player class constructor
	/*playerData.setPlayerOneBalance(1000.0f);
	playerData.setPlayerTwoBalance(1000.0f);*/

	// Change the Price of the car here, if you want to set the name of the car, use the same format as price
	playerData.playerCar[0].setPrice(500.0f);
	playerData.playerCar[1].setPrice(750.0f);
	playerData.playerCar[2].setPrice(1000.0f);
	playerData.playerCar[3].setPrice(100.0f);
	playerData.playerCar[4].setPrice(500.0f);
	playerData.playerCar[5].setPrice(750.0f);
	playerData.playerCar[6].setPrice(1000.0f);
	playerData.playerCar[7].setPrice(100.0f);

	playerData.player1selectedcar = 0;
	playerData.player2selectedcar = 4;

	toggleBoundingBox = true;
	toggleFortuneWheel = false;
	fortuneWheelPrizeGiven = true;
	fortuneWheelSelection = 1;
}

void SceneText::Update(double dt)
{
	Vector3 vNPC(ObjectList.NPC.getTranslationX(), ObjectList.NPC.getTranslationY(), ObjectList.NPC.getTranslationZ());
	Vector3 vFortuneWheel(ObjectList.fortuneWheel.getTranslationX(), ObjectList.fortuneWheel.getTranslationY(), ObjectList.fortuneWheel.getTranslationZ());
	currentTime += dt;

	keyPressed = false;
	abletoPress = true;
	elapsedTime += dt;

	if (bounceTime > elapsedTime)
	{
		abletoPress = false;
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
	{
		light[0].position.y -= (float)(LSPEED * dt);
	}
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
	currentTime += dt;

	// For Shop Interface

	if (Application::IsKeyPressed('G') && (camera.position - vNPC).Length() <= 20 && abletoPress) // Toggle Shop GUI when NPC is near you
	{
		if (toggleShop)
		{
			toggleShop = false;
		}
		else
		{
			toggleShop = true;
		}

		keyPressed = true;
	}

	if ((camera.position - vNPC).Length() > 20) // If NPC out of range
	{
		toggleShop = false;
	}

	if (Application::IsKeyPressed('L') && abletoPress)
	{
		if (toggleBoundingBox)
		{
			toggleBoundingBox = false;
		}
		else
		{
			toggleBoundingBox = true;
		}

		keyPressed = true;
	}

	if (Application::IsKeyPressed(VK_DOWN) && selection != 5 && abletoPress)
	{
		selection += 1;
		movedown = true;
		keyPressed = true;
	}
	if (Application::IsKeyPressed(VK_UP) && selection != 1 && abletoPress)
	{
		selection -= 1;
		moveup = true;
		keyPressed = true;
	}

	ytranslate += 0.2;
	if (ytranslate >= 10.f) //ring animation
	{
		ytranslate = 0.f;
	}
	ObjectList.ring1.setTranslationY(ytranslate);

	if (camera.position.x > 0)
	{
		lightposx = 48.f;
		ringposx = 55.f;
		if (camera.position.z > 0)
		{
			lightposz = 48.f;
			ringposz = 55.f;
		}
		else
		{
			lightposz = -48.f;
			ringposz = -55.f;
		}
	}
	else
	{
		lightposx = -48.f;
		ringposx = -55.f;
		if (camera.position.z > 0)
		{
			lightposz = 48.f;
			ringposz = 55.f;
		}
		else
		{
			lightposz = -48.f;
			ringposz = -55.f;
		}
	}

	ytranslate2 += 0.5;
	if (ytranslate2 >= 50.f) //animation of turntable ring
	{
		ytranslate2 = 0.f;
	}

	light[1].position.Set(lightposx, 50, lightposz); //update light position
	ObjectList.ring2.setTranslationXYZ(ringposx, ytranslate2, ringposz); //update turntable ring pos

	rotation1++;
	ObjectList.turntable1.setRotationAmount(rotation1); //turntable rotation
	ObjectList.turntable2.setRotationAmount(rotation1);
	ObjectList.newcar1.setRotationAmount(rotation1);
	ObjectList.newcar3.setRotationAmount(rotation1);
	rotation2--;
	ObjectList.turntable3.setRotationAmount(rotation2);
	ObjectList.turntable4.setRotationAmount(rotation2);
	ObjectList.newcar2.setRotationAmount(rotation2);
	ObjectList.newcar4.setRotationAmount(rotation2);

	if ((camera.position - vFortuneWheel).Length() <= 35)
	{
		toggleFortuneWheel = true;
	}
	else
	{
		toggleFortuneWheel = false;
	}

	if (toggleFortuneWheel)
	{
		FortuneWheel(dt);
	}

	if (Application::IsKeyPressed('Y') && abletoPress) {
		//cycles the texture of whichever car you are at
		if (texture == 3) {
			texture = 0;
		}
		texture++;
		if (atTurntable == 1) {
			if (perviousLocation != atTurntable) {
				perviousLocation = atTurntable;
				texture = 1;
			}
			if (texture == 1)
				meshList[GEO_CAR1]->textureID = LoadTGA("Image//newcar.tga");
			if (texture == 2)
				meshList[GEO_CAR1]->textureID = LoadTGA("Image//newcarskin2.tga");
			if (texture == 3)
				meshList[GEO_CAR1]->textureID = LoadTGA("Image//newcarskin3.tga");

			meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture.tga");
			meshList[GEO_CAR3]->textureID = LoadTGA("Image//newcar3.tga");
			meshList[GEO_CAR4]->textureID = LoadTGA("Image//newcar.tga");
		}
		if (atTurntable == 2) {
			if (perviousLocation != atTurntable) {
				perviousLocation = atTurntable;
				texture = 1;
			}
			if (texture == 1)
				meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture.tga");
			if (texture == 2)
				meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture 2.tga");
			if (texture == 3)
				meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture 3.tga");

			meshList[GEO_CAR1]->textureID = LoadTGA("Image//newcar.tga");
			meshList[GEO_CAR3]->textureID = LoadTGA("Image//newcar3.tga");
			meshList[GEO_CAR4]->textureID = LoadTGA("Image//newcar.tga");
			
		}
		if (atTurntable == 3) {
			if (perviousLocation != atTurntable) {
				perviousLocation = atTurntable;
				texture = 1;
			}
			if (texture == 1)
				meshList[GEO_CAR3]->textureID = LoadTGA("Image//newcar3.tga");
			if (texture == 2)
				meshList[GEO_CAR3]->textureID = LoadTGA("Image//newcar3skin2.tga");
			if (texture == 3)
				meshList[GEO_CAR3]->textureID = LoadTGA("Image//newcar3skin3.tga");

			meshList[GEO_CAR1]->textureID = LoadTGA("Image//newcar.tga");
			meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture.tga");
			meshList[GEO_CAR4]->textureID = LoadTGA("Image//newcar.tga");
			
		}
		if (atTurntable == 4) {
			if (perviousLocation != atTurntable) {
				perviousLocation = atTurntable;
				texture = 1;
			}
			if (texture == 1)
				meshList[GEO_CAR4]->textureID = LoadTGA("Image//newcar.tga");
			if (texture == 2)
				meshList[GEO_CAR4]->textureID = LoadTGA("Image//newcarskin2.tga");
			if (texture == 3)
				meshList[GEO_CAR4]->textureID = LoadTGA("Image//newcarskin3.tga");

			meshList[GEO_CAR1]->textureID = LoadTGA("Image//newcar.tga");
			meshList[GEO_CAR2]->textureID = LoadTGA("Image//192206L_KohKaiYang_A2_car texture.tga");
			meshList[GEO_CAR3]->textureID = LoadTGA("Image//newcar3.tga");
		}
	}
	
	carShowInteraction(dt);
	camera.MouseControl();
	cameraMovement(dt);
	CalculateFrameRate();
	playerData.updateFile();

	//ObjectList.Character = replay->Replay();

	if (keyPressed)
	{
		bounceTime = elapsedTime + 0.1;
	}
}

bool SceneText::skyboxcheck()
{
	if (camera.position.x > 83 && camera.position.x < 100 && camera.position.z < 10 && camera.position.z > -10)
	{
		return true;
	}
}

void SceneText::Render()
{

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z, camera.target.x, camera.target.y, camera.target.z, camera.up.x, camera.up.y, camera.up.z);
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

	if (light[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	// if it is spot light, pass in position and direction 
	else if (light[1].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[1].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		// default is point light (only position since point light is 360 degrees)
		Position lightPosition_cameraspace = viewStack.Top() * light[1].position;
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	}

	RenderSkybox();
	RenderNPC();

	Vector3 lightPos;
	lightPos.x = light[0].position.x;
	lightPos.y = light[0].position.y;
	lightPos.z = light[0].position.z;

	if (toggleBoundingBox)
	{
		RenderBoundingBox();
	}

	modelStack.PushMatrix();
	modelStack.Translate(lightPos.x, lightPos.y, lightPos.z);
	RenderMesh(meshList[GEO_LIGHTSPHERE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	RenderMesh(meshList[GEO_LIGHTSPHERE2], false);
	modelStack.PopMatrix();

	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(ObjectList.fortuneWheel.getRotationAmount()), Color(0, 1, 0), 1.75, 15, 24);

	//=====================Shop Interface============================================
	//RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(playerData.playerCar[0].getUnlocked()), Color(0, 1, 0), 1.75, 13, 28);
	RenderTextOnScreen(meshList[GEO_TEXT], "P1 Balance:", Color(0, 1, 0), 2, 0, 28);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(playerData.getPlayerOneBalance()), Color(0, 1, 0), 1.75, 13, 32);
	RenderTextOnScreen(meshList[GEO_TEXT], "P2 Balance:", Color(0, 1, 0), 2, 0, 27);
	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(playerData.getPlayerTwoBalance()), Color(0, 1, 0), 1.75, 13, 30.8);

}

void SceneText::Exit()
{
	// Cleanup here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i] != NULL)
			delete meshList[i];
	}
	replay[0].resetReplay();
	replay[1].resetReplay();

	// Stop music
	playMusic->setIsPaused();
	playMusic->drop();
	SoundEngine->drop();

	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneText::RenderMesh(Mesh* mesh, bool enableLight)
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

void SceneText::RenderObject(Mesh* mesh, Object meshObject, bool enableLight)
{
	modelStack.PushMatrix();
	modelStack.Translate(meshObject.getTranslationX(), meshObject.getTranslationY(), meshObject.getTranslationZ());
	modelStack.Scale(meshObject.getScaleX(), meshObject.getScaleY(), meshObject.getScaleZ());
	modelStack.Rotate(meshObject.getRotationAmount(), meshObject.getRotateX(), meshObject.getRotateY(), meshObject.getRotateZ());
	RenderMesh(mesh, enableLight);
	modelStack.PopMatrix();
}

void SceneText::RenderSkybox()
{
	RenderObject(meshList[GEO_LEFT], ObjectList.leftwall, true);
	RenderObject(meshList[GEO_RIGHT], ObjectList.rightwall, true);
	RenderObject(meshList[GEO_TOP], ObjectList.roof, true);
	RenderObject(meshList[GEO_BOTTOM], ObjectList.floor, true);
	RenderObject(meshList[GEO_FRONT], ObjectList.frontwall, true);
	RenderObject(meshList[GEO_BACK], ObjectList.backwall, true);

	RenderObject(meshList[GEO_LEFT2], ObjectList.leftwall2, false);
	RenderObject(meshList[GEO_RIGHT2], ObjectList.rightwall2, false);
	RenderObject(meshList[GEO_TOP2], ObjectList.roof2, false);
	RenderObject(meshList[GEO_FRONT2], ObjectList.frontwall2, false);
	RenderObject(meshList[GEO_BACK2], ObjectList.backwall2, false);

	RenderObject(meshList[GEO_PILLAR], ObjectList.pillar1, true);
	RenderObject(meshList[GEO_PILLAR2], ObjectList.pillar2, true);
	RenderObject(meshList[GEO_PILLAR3], ObjectList.pillar3, true);
	RenderObject(meshList[GEO_PILLAR4], ObjectList.pillar4, true);

	RenderObject(meshList[GEO_RING], ObjectList.ring1, true);
	RenderObject(meshList[GEO_RING2], ObjectList.ring2, true);

	RenderObject(meshList[GEO_TURNTABLE1], ObjectList.turntable1, true);
	RenderObject(meshList[GEO_TURNTABLE2], ObjectList.turntable2, true);
	RenderObject(meshList[GEO_TURNTABLE3], ObjectList.turntable3, true);
	RenderObject(meshList[GEO_TURNTABLE4], ObjectList.turntable4, true);

	RenderObject(meshList[GEO_CAR1], ObjectList.newcar1, false);
	RenderObject(meshList[GEO_CAR2], ObjectList.newcar2, false);
	RenderObject(meshList[GEO_CAR3], ObjectList.newcar3, false);
	RenderObject(meshList[GEO_CAR4], ObjectList.newcar4, false);

	RenderObject(meshList[GEO_FORTUNEWHEEL], ObjectList.fortuneWheel, false);
	RenderObject(meshList[GEO_FORTUNEWHEEL_INDICATOR], ObjectList.fortuneWheelIndicator, false);
}

void SceneText::VerticeUpdate(Mesh* mesh, Object meshObject)
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
}

void SceneText::VerticeUpdateNPC()
{
	if (ObjectList.NPC.getLastTranslationX() != ObjectList.NPC.getTranslationX() || ObjectList.NPC.getLastTranslationY() != ObjectList.NPC.getTranslationY() || ObjectList.NPC.getLastTranslationZ() != ObjectList.NPC.getTranslationZ()) // Updates the Bounding Box 8-vertices.
	{
		for (int counter = 0; counter <= 7; counter++)
		{
			meshList[GEO_NPC]->vertices[counter].x = meshList[GEO_NPC]->vertices[counter].x - ObjectList.NPC.getLastTranslationX() + ObjectList.NPC.getTranslationX();
			meshList[GEO_NPC]->vertices[counter].y = meshList[GEO_NPC]->vertices[counter].y - ObjectList.NPC.getLastTranslationY() + ObjectList.NPC.getTranslationY();
			meshList[GEO_NPC]->vertices[counter].z = meshList[GEO_NPC]->vertices[counter].z - ObjectList.NPC.getLastTranslationZ() + ObjectList.NPC.getTranslationZ();
		}

		meshList[GEO_NPC]->maxX = meshList[GEO_NPC]->vertices[6].x, meshList[GEO_NPC]->maxY = meshList[GEO_NPC]->vertices[6].y, meshList[GEO_NPC]->maxZ = meshList[GEO_NPC]->vertices[6].z, meshList[GEO_NPC]->minX = meshList[GEO_NPC]->vertices[0].x, meshList[GEO_NPC]->minY = meshList[GEO_NPC]->vertices[0].y, meshList[GEO_NPC]->minZ = meshList[GEO_NPC]->vertices[0].z;
		ObjectList.NPC.setLastTranslationX(ObjectList.NPC.getTranslationX());
		ObjectList.NPC.setLastTranslationY(ObjectList.NPC.getTranslationY());
		ObjectList.NPC.setLastTranslationZ(ObjectList.NPC.getTranslationZ());
	}
}

void SceneText::carShowInteraction(double dt)
{
	float NPC_SPEED = 50.f;
	int translateX = 0;
	int translateZ = 0;
	Vector3 vTurntable1(ObjectList.turntable1.getTranslationX(), ObjectList.turntable1.getTranslationY(), ObjectList.turntable1.getTranslationZ());
	Vector3 vTurntable2(ObjectList.turntable2.getTranslationX(), ObjectList.turntable2.getTranslationY(), ObjectList.turntable2.getTranslationZ());
	Vector3 vTurntable3(ObjectList.turntable3.getTranslationX(), ObjectList.turntable3.getTranslationY(), ObjectList.turntable3.getTranslationZ());
	Vector3 vTurntable4(ObjectList.turntable4.getTranslationX(), ObjectList.turntable4.getTranslationY(), ObjectList.turntable4.getTranslationZ());
	Vector3 vNPC(ObjectList.NPC.getTranslationX(), ObjectList.NPC.getTranslationY(), ObjectList.NPC.getTranslationZ());
	
	if ((camera.position - vTurntable1).Length() <= (camera.position - vTurntable2).Length() &&
		(camera.position - vTurntable1).Length() <= (camera.position - vTurntable3).Length() && 
		(camera.position - vTurntable1).Length() <= (camera.position - vTurntable4).Length())
	{
		atTurntable = 1;
	}
	else if ((camera.position - vTurntable2).Length() <= (camera.position - vTurntable1).Length() && 
			 (camera.position - vTurntable2).Length() <= (camera.position - vTurntable3).Length() && 
			 (camera.position - vTurntable2).Length() <= (camera.position - vTurntable4).Length())
	{
		atTurntable = 2;
	}
	else if ((camera.position - vTurntable3).Length() <= (camera.position - vTurntable1).Length() && 
		     (camera.position - vTurntable3).Length() <= (camera.position - vTurntable2).Length() && 
		     (camera.position - vTurntable3).Length() <= (camera.position - vTurntable4).Length())
	{
		atTurntable = 3;
	}
	else if ((camera.position - vTurntable4).Length() <= (camera.position - vTurntable1).Length() && 
		     (camera.position - vTurntable4).Length() <= (camera.position - vTurntable2).Length() && 
		     (camera.position - vTurntable4).Length() <= (camera.position - vTurntable3).Length())
	{
		atTurntable = 4;
	}

	switch (atTurntable)
	{
		case 1:
			translateX = 20;
			translateZ = 20;
			break;
		case 2:
			translateX = 20;
			translateZ = -18;
			break;
		case 3:
			translateX = -30;
			translateZ = 20;
			break;
		case 4:
			translateX = -30;
			translateZ = -18;
			break;
	}

	if (ObjectList.NPC.getTranslationX() <= translateX)
	{
		ObjectList.NPC.setTranslationX(ObjectList.NPC.getTranslationX() + NPC_SPEED * dt);
		VerticeUpdateNPC();
		if (ObjectList.NPC.getTranslationX() > translateX)
		{
			ObjectList.NPC.setTranslationX(translateX);
			VerticeUpdateNPC();
		}
	}
	else
	{
		ObjectList.NPC.setTranslationX(ObjectList.NPC.getTranslationX() - NPC_SPEED * dt);
		VerticeUpdateNPC();
		if (ObjectList.NPC.getTranslationX() < translateX)
		{
			ObjectList.NPC.setTranslationX(translateX);
			VerticeUpdateNPC();
		}
	}

	if (ObjectList.NPC.getTranslationZ() <= translateZ)
	{
		ObjectList.NPC.setTranslationZ(ObjectList.NPC.getTranslationZ() + NPC_SPEED * dt);
		VerticeUpdateNPC();
		if (ObjectList.NPC.getTranslationZ() > translateZ)
		{
			ObjectList.NPC.setTranslationZ(translateZ);
			VerticeUpdateNPC();
		}
	}
	else
	{
		ObjectList.NPC.setTranslationZ(ObjectList.NPC.getTranslationZ() - NPC_SPEED * dt);
		VerticeUpdateNPC();
		if (ObjectList.NPC.getTranslationZ() < translateZ)
		{
			ObjectList.NPC.setTranslationZ(translateZ);
			VerticeUpdateNPC();
		}
	}

	if (toggleShop)
	{
		switch (atTurntable)
		{
		case 1:
			ShopUI(atTurntable - 1);
			break;
		case 2:
			ShopUI(atTurntable - 1);
			break;
		case 3:
			ShopUI(atTurntable - 1);
			break;
		case 4:
			ShopUI(atTurntable - 1);
			break;
		}
	}
}

void SceneText::FortuneWheel(double dt)
{
	if (Application::IsKeyPressed(VK_DOWN) && fortuneWheelSelection == 1)
	{
		fortuneWheelSelection = 2;
	}

	if (Application::IsKeyPressed(VK_UP) && fortuneWheelSelection == 2)
	{
		fortuneWheelSelection = 1;
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "Cost: 500", Color(1, 1, 1), 3, 0, 3);
	if (fortuneWheelSelection == 1)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Spin Fortune Wheel as Player 1", Color(0, 1, 0), 2, 0, 2);
		RenderTextOnScreen(meshList[GEO_TEXT], "Spin Fortune Wheel as Player 2", Color(1, 1, 1), 2, 0, 1);
	}
	else if (fortuneWheelSelection == 2)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Spin Fortune Wheel as Player 1", Color(1, 1, 1), 2, 0, 2);
		RenderTextOnScreen(meshList[GEO_TEXT], "Spin Fortune Wheel as Player 2", Color(0, 1, 0), 2, 0, 1);
	}

	if (Application::IsKeyPressed('F') && !isSpinning) // Activates the fortune wheel, sets a random angle to spin till using rng.
	{
		if (fortuneWheelSelection == 1)
		{
			playerData.setPlayerOneBalance(playerData.getPlayerOneBalance() - 500);
		}
		else if (fortuneWheelSelection == 2)
		{
			playerData.setPlayerTwoBalance(playerData.getPlayerTwoBalance() - 500);
		}

		ObjectList.fortuneWheel.setRotationAmount(0);
		rotationSpeed = Math::RandIntMinMax(800, 1600);
		isSpinning = true;
	}

	if (isSpinning) // During the spinning process
	{

		ObjectList.fortuneWheel.setRotationAmount(ObjectList.fortuneWheel.getRotationAmount() + dt * rotationSpeed);
		rotationSpeed -= 10;

		if (rotationSpeed <= 0)
		{
			rotationSpeed = 0;
			isSpinning = false;
		}
	}

	if (!isSpinning)// When it's done spinning
	{
		if (ObjectList.fortuneWheel.getRotationAmount() > 360)
		{
			ObjectList.fortuneWheel.setRotationAmount(ObjectList.fortuneWheel.getRotationAmount() - 360);
			fortuneWheelPrizeGiven = false;
		}

		if (ObjectList.fortuneWheel.getRotationAmount() < 0)
		{
			ObjectList.fortuneWheel.setRotationAmount(ObjectList.fortuneWheel.getRotationAmount() + 360);
			fortuneWheelPrizeGiven = false;
		}

		if (!fortuneWheelPrizeGiven)
		{
			float rotationValue = ObjectList.fortuneWheel.getRotationAmount();
			float result = 0;

			if ((rotationValue > 0 && rotationValue < 15) || (rotationValue < 360 && rotationValue > 345))
			{
				result = -500;
			}
			else if (rotationValue > 15 && rotationValue < 45)
			{
				result = 750;
			}
			else if (rotationValue > 45 && rotationValue < 75)
			{
				result = 300;
			}
			else if (rotationValue > 75 && rotationValue < 105)
			{
				if (fortuneWheelSelection == 1)
				{
					playerData.setPlayerOneBalance(2 * playerData.getPlayerOneBalance());
				}
				else if (fortuneWheelSelection == 2)
				{
					playerData.setPlayerTwoBalance(2 * playerData.getPlayerTwoBalance());
				}
			}
			else if (rotationValue > 105 && rotationValue < 135)
			{
				result = 500;
			}
			else if (rotationValue > 135 && rotationValue < 165)
			{
				result = 1000;
			}
			else if (rotationValue > 165 && rotationValue < 195)
			{
				result = 100;
			}
			else if (rotationValue > 195 && rotationValue < 225)
			{
				result = -250;
			}
			else if (rotationValue > 225 && rotationValue < 255)
			{
				result = 2000;
			}
			else if (rotationValue > 255 && rotationValue < 285)
			{
				if (fortuneWheelSelection == 1)
				{
					playerData.setPlayerOneBalance(0);
				}
				else if (fortuneWheelSelection == 2)
				{
					playerData.setPlayerTwoBalance(0);
				}
			}
			else if (rotationValue > 285 && rotationValue < 315)
			{
				result = -1000;
			}
			else if (rotationValue > 315 && rotationValue < 345)
			{
				result = 5000;
			}

			if (fortuneWheelSelection == 1)
			{
				playerData.setPlayerOneBalance(playerData.getPlayerOneBalance() + result);
			}
			else if (fortuneWheelSelection == 2)
			{
				playerData.setPlayerTwoBalance(playerData.getPlayerTwoBalance() + result);
			}

			fortuneWheelPrizeGiven = true;
		}
	}
}

void SceneText::RenderNPC() // Code related to NPC
{
	modelView.x = 0; // A general direction for the NPC to dot product from
	modelView.y = 0;
	modelView.z = -1;
	modelView.Normalize();
	targetView.x = camera.position.x - ObjectList.NPC.getTranslationX(); // Get the x vector from the NPC to player
	targetView.y = 0;
	targetView.z = camera.position.z - ObjectList.NPC.getTranslationZ(); // Get the z vector from the NPC to player
	targetView.Normalize();
	dotProduct = modelView.Dot(targetView); // Dot Product between the general direction vector and the vector pointing from the NPC to the player

	rotation = Math::RadianToDegree(acos(dotProduct / (modelView.Length() * targetView.Length()))); // Calculates the rotation in degrees from the player to the general direction

	if (camera.position.x <= ObjectList.NPC.getTranslationX()) // If the player is on the leftside
	{
		ObjectList.NPC.setRotationAmount(rotation - 180);
	}
	else // If player is on right side of the NPC
	{
		ObjectList.NPC.setRotationAmount(180 - rotation);
	}

	RenderObject(meshList[GEO_NPC], ObjectList.NPC, false); // Renders the NPC
	VerticeUpdateNPC();
}

void SceneText::RenderBoundingBox()
{
	meshList[GEO_TURNTABLE1_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_TURNTABLE1]->vertices);
	meshList[GEO_TURNTABLE2_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_TURNTABLE2]->vertices);
	meshList[GEO_TURNTABLE3_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_TURNTABLE3]->vertices);
	meshList[GEO_TURNTABLE4_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_TURNTABLE4]->vertices);
	meshList[GEO_PILLAR_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_PILLAR]->vertices);
	meshList[GEO_PILLAR2_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_PILLAR2]->vertices);
	meshList[GEO_PILLAR3_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_PILLAR3]->vertices);
	meshList[GEO_PILLAR4_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_PILLAR4]->vertices);
	meshList[GEO_FORTUNEWHEEL_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_FORTUNEWHEEL]->vertices);
	meshList[GEO_NPC_BOX] = MeshBuilder::GenerateBoundingBox("boundingbox", meshList[GEO_NPC]->vertices);

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_TURNTABLE1_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_TURNTABLE2_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_TURNTABLE3_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_TURNTABLE4_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PILLAR_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PILLAR2_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PILLAR3_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_PILLAR4_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_NPC_BOX], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_FORTUNEWHEEL_BOX], false);
	modelStack.PopMatrix();

}

void SceneText::RenderText(Mesh* mesh, std::string text, Color color)
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

void SceneText::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(sizex, sizey, 1);


	RenderMesh(mesh, false);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
}

void SceneText::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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

void SceneText::cameraMovement(double dt)
{
	Vector3 view = (camera.target - camera.position).Normalized();
	Vector3 right = view.Cross(camera.up);
	float MOVEMENT_SPEED = 15.f;

	if (Application::IsKeyPressed('A'))
	{
		camera.position -= right * (float)(MOVEMENT_SPEED * dt);
		if (ObjectList.PointToMeshCollision(camera.position, meshList[GEO_FORTUNEWHEEL]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE1]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE2]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE3]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE4]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_NPC]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_LEFT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_RIGHT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_BACK]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_FRONT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR2]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR3]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR4]))
		{
			camera.position += right * (float)(MOVEMENT_SPEED * dt);
		}
	}
	if (Application::IsKeyPressed('D'))
	{
		camera.position += right * (float)(MOVEMENT_SPEED * dt);
		if (ObjectList.PointToMeshCollision(camera.position, meshList[GEO_FORTUNEWHEEL]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE1]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE2]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE3]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE4]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_NPC]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_LEFT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_RIGHT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_BACK]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_FRONT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR2]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR3]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR4]))
		{
			camera.position -= right * (float)(MOVEMENT_SPEED * dt);
		}

	}
	if (Application::IsKeyPressed('W'))
	{
		camera.position += view * (float)(MOVEMENT_SPEED * dt);
		if (ObjectList.PointToMeshCollision(camera.position, meshList[GEO_FORTUNEWHEEL]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE1]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE2]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE3]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE4]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_NPC]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_LEFT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_RIGHT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_BACK]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_FRONT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR2]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR3]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR4]))
		{
			camera.position -= view * (float)(MOVEMENT_SPEED * dt);
		}
	}
	if (Application::IsKeyPressed('S'))
	{
		camera.position -= view * (float)(MOVEMENT_SPEED * dt);
		if (ObjectList.PointToMeshCollision(camera.position, meshList[GEO_FORTUNEWHEEL]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE1]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE2]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE3]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_TURNTABLE4]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_NPC]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_LEFT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_RIGHT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_BACK]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_FRONT]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR2]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR3]) ||
			ObjectList.PointToMeshCollision(camera.position, meshList[GEO_PILLAR4]))
		{
			camera.position += view * (float)(MOVEMENT_SPEED * dt);
		}
	}

	camera.position.y = 15;
	camera.target = camera.position + view;
}

void SceneText::CalculateFrameRate()
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

void SceneText::RenderLeftScreen()
{
	SceneText::Render();
}

void SceneText::RenderRightScreen()
{
	SceneText::Render();
}

bool SceneText::SwitchScene()
{
	if (ObjectList.PointToMeshCollision(camera.position, meshList[GEO_RING]))
		return true;
	else
		return false;

}

void SceneText::ShopUI(int carnum)
{
	if (selection == 1) // if indicator pointing on first option
		optionselected[0] = true;
	else
		optionselected[0] = false;

	if (selection == 2) // if indicator pointing on second option
		optionselected[1] = true;
	else
		optionselected[1] = false;

	if (selection == 3) // if indicator pointing on third option
		optionselected[2] = true;
	else
		optionselected[2] = false;
	if (selection == 4) // if indicator pointing on forth option(player 1 texture)
		optionselected[3] = true;
	else
		optionselected[3] = false;
	if (selection == 5) // if indicator pointing on fifth option(player 2 texture)
		optionselected[4] = true;
	else
		optionselected[4] = false;


	// Use this to display prices & names of cars later on
	modelStack.PushMatrix();
	RenderText(meshList[GEO_TEXT], std::to_string(playerData.playerCar[carnum].getPrice()), Color(0, 1, 0));
	modelStack.PopMatrix();

	RenderMeshOnScreen(meshList[GEO_TEXTBG], 24.5, 7.5, 56, 17);

	if (playerData.playerCarUnlocked[carnum] && playerData.player1selectedcar != carnum) // if the car is unlocked but not selected
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Select Car for Player 1", Color(1, 1, 1), 2, 0, 6);
	}
	else if (playerData.playerCarUnlocked[carnum] && playerData.player1selectedcar == carnum) // if car is selected 
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Car Selected for Player 1", Color(1, 1, 1), 2, 0, 6);
	}
	else if (!playerData.playerCarUnlocked[carnum]) // if car is still locked
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Purchase Car as Player 1", Color(1, 1, 1), 2, 0, 6);
	}
	if (playerData.playerCarUnlocked[carnum + 4] && playerData.player2selectedcar != carnum + 4) // refer above^
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Select Car for Player 2", Color(1, 1, 1), 2, 0, 5);
	}
	else if (playerData.playerCarUnlocked[carnum + 4] && playerData.player2selectedcar == carnum + 4)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Car Selected for Player 2", Color(1, 1, 1), 2, 0, 5);
	}
	else if (!playerData.playerCarUnlocked[carnum + 4])
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Purchase Car as Player 2", Color(1, 1, 1), 2, 0, 5);
	}


	RenderTextOnScreen(meshList[GEO_TEXT], "Test Drive", Color(1, 1, 1), 2, 0, 4);

	RenderTextOnScreen(meshList[GEO_TEXT], "Select skin for player 1", Color(1, 1, 1), 2, 0, 3);

	RenderTextOnScreen(meshList[GEO_TEXT], "Select skin for player 2", Color(1, 1, 1), 2, 0, 2);

	printIndicator(carnum);

	if (Application::IsKeyPressed(VK_RETURN) && playerData.getPlayerOneBalance() >= playerData.playerCar[carnum].getPrice() && !playerData.playerCarUnlocked[carnum] && abletoPress && optionselected[0]) // when purchase car
	{

		playerData.setPlayerOneBalance(playerData.getPlayerOneBalance() - playerData.playerCar[carnum].getPrice());
		playerData.playerCarUnlocked[carnum] = true;
		playerData.updateFile();
		keyPressed = true;
	}
	if (Application::IsKeyPressed(VK_RETURN) && playerData.playerCarUnlocked[carnum] && abletoPress && optionselected[0]) // selection of car
	{
		playerData.player1selectedcar = carnum;
		keyPressed = true;
	}
	if (Application::IsKeyPressed(VK_RETURN) && playerData.getPlayerTwoBalance() >= playerData.playerCar[carnum + 4].getPrice() && !playerData.playerCarUnlocked[carnum + 4] && abletoPress && optionselected[1])
	{
		playerData.setPlayerTwoBalance(playerData.getPlayerTwoBalance() - playerData.playerCar[carnum + 4].getPrice());
		playerData.playerCarUnlocked[carnum + 4] = true;
		playerData.updateFile();
		keyPressed = true;
	}
	if (Application::IsKeyPressed(VK_RETURN) && playerData.playerCarUnlocked[carnum + 4] && abletoPress && optionselected[1])
	{
		playerData.player2selectedcar = carnum + 4;
		keyPressed = true;
	}
	if (Application::IsKeyPressed(VK_RETURN) && playerData.playerCarUnlocked[carnum] && abletoPress && optionselected[3])
	{
		playerData.player1Texture = texture;
	}
	if (Application::IsKeyPressed(VK_RETURN) && playerData.playerCarUnlocked[carnum + 4] && abletoPress && optionselected[4])
	{
		playerData.player2Texture = texture;
	}
	if (keyPressed == true && abletoPress == true && optionselected[2] == true)
	{
		// Change the Scene to Test Drive here
	}
	if (playerData.getPlayerOneBalance() < playerData.playerCar[carnum].getPrice() && optionselected[0] == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "You do not have enough money", Color(0, 1, 0), 2, 0, 26);
	}
	if (playerData.getPlayerTwoBalance() < playerData.playerCar[carnum + 4].getPrice() && optionselected[1] == true)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "You do not have enough money", Color(0, 1, 0), 2, 0, 26);
	}
}

void SceneText::printIndicator(int carnum)
{

	if (selection == 1)
	{
		if (playerData.player1selectedcar == carnum)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(1, 0, 0), 2, 25, 6);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(1, 0, 0), 2, 24, 6);
		}
	}
	else if (selection == 2)
	{
		if (playerData.player2selectedcar == carnum + 4)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(1, 0, 0), 2, 25, 5);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(1, 0, 0), 2, 24, 5);
		}
	}
	else if (selection == 3)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(1, 0, 0), 2, 10, 4);
	}
	else if (selection == 4)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(1, 0, 0), 2, 24, 3);
	}
	else if (selection == 5)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(1, 0, 0), 2, 24, 2);
	}

}

void SceneText::Reset()
{
	replay[0].resetReplay();
	replay[1].resetReplay();

	// Stop music
	playMusic->setIsPaused();
}