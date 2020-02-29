#ifndef SCENERACE_H
#define SCENERACE_H

#include "Scene.h"
#include <MatrixStack.h>
#include "Camera2.h"
#include "Mesh.h"
#include "Light.h"
#include "Acceleration.h"
#include "ObjectManager.h"
#include <math.h>
#include <irrKlang.h>
#include "Cars.h"
#include "Player.h"
#include "ReplayRace.h"
#pragma comment(lib, "irrKlang.lib")

class SceneRace : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_CAR1,
		GEO_CAR2,
		GEO_CAR3,
		GEO_CAR4,
		GEO_PLAYERONE,
		GEO_PLAYERTWO,
		GEO_RACETRACK,
		GEO_OBSTACLE1,
		GEO_OBSTACLE2,
		GEO_OBSTACLE3,
		GEO_OBSTACLE4,
		GEO_BOUNDARY,
		GEO_TEXT,
		GEO_LIGHTSPHERE,
		GEO_LIGHTSPHERE2,
		NUM_GEOMETRY,
	};

	enum Scene5_UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		//add these enum in UNIFORM_TYPE before U_TOTAL
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		U_NUMLIGHTS,
		// add these enum for texture
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

private:
	unsigned m_vertexArrayID;
	unsigned m_programID;
	/*unsigned m_indexBuffer;*/
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_parameters[U_TOTAL];

	MS modelStack, viewStack, projectionStack;
	
	Light light[2];

	Vector3 playerPos;
	Vector3 playerTwoPos;

	Camera2 camera[2];

	ReplayRace replay[2];

	irrklang::ISoundEngine* SoundEngine;
	irrklang::ISoundSource* ambience;
	irrklang::ISoundSource* wind;
	irrklang::ISoundSource* car;
	irrklang::ISound* playStart;
	irrklang::ISound* playBG;
	irrklang::ISound* playWind;
	irrklang::ISound* playCar;
	irrklang::ISound* playCarTwo;

	int screen;
	float time;
	float playerOneYaw;
	float playerTwoYaw;
	Vector3 forwardDirection;
	Vector3 backwardDirection;
	Vector3 forwardTwoDirection;

	bool initTimer;
	double elapsedTime;
	double startTime;


	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderObject(Mesh* mesh, Object meshObject, bool enableLight);
	void RenderSkybox();
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void startTimer();
	string CalculateTime();

public:
	SceneRace();
	~SceneRace();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	virtual void RenderLeftScreen();
	virtual void RenderRightScreen();
	virtual bool SwitchScene();
	virtual bool skyboxcheck();
	virtual void Reset();
	void asda();
};

#endif 