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

	ObjectList.Character.init(0, 0, 0, 1, 1, 1, 0, 0, 1, 0);
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

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//left.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//right.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//top.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//front.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//back.tga");

	meshList[GEO_CHAR] = MeshBuilder::GenerateQuad("char", Color(1, 1, 1), 1.f, 1.f, 0, 0, 0);
	meshList[GEO_CHAR]->textureID = LoadTGA("Image//char.tga");

	meshList[GEO_DICE] = MeshBuilder::GenerateOBJ("Dice","OBJ//doorman.obj", ObjectList.Character.getOffsetX(), ObjectList.Character.getOffsetY(), ObjectList.Character.getOffsetZ());
	meshList[GEO_DICE]->textureID = LoadTGA("Image//doorman.tga");

	meshList[GEO_LIGHTSPHERE] = MeshBuilder::GenerateSphere("lightBall", Color(1.f, 1.f, 1.f), 9, 36, 1.f, 0, 5, 0);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//calibri.tga");

	currentTime = 0;
	
}

void SceneText::Update(double dt)
{
	currentTime += dt;
	isAcceleratingA = false;
	isAcceleratingB = false;
	isDeceleratingA = false;
	isDeceleratingB = false;

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

	//camera[screen].Init(camera[screen].position , playerPos, Vector3(0, 1, 0), 0);  // option 2 for 3rd person cam

	//camera.MouseControl();
	camera[screen].Update(dt);
	CalculateFrameRate();
}

void SceneText::Render()
{
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

	modelStack.PushMatrix();
	//scale, translate, rotate
	RenderText(meshList[GEO_TEXT], "HELLO WORLD", Color(0, 1, 0));
	modelStack.PopMatrix();

	//No transform needed
	RenderTextOnScreen(meshList[GEO_TEXT], "Hello World", Color(0, 1, 0), 2, 0, 0);

}

void SceneText::Exit()
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
	screen = 0;
	SceneText::Render();
}

void SceneText::RenderRightScreen()
{
	screen = 1;
	SceneText::Render();
}

int switchscreen = 0;

int SceneText::SwitchScene()
{
	if (Application::IsKeyPressed('P'))
		switchscreen = 1;
	else if (Application::IsKeyPressed('O'))
		switchscreen = 0;

	return switchscreen;
}