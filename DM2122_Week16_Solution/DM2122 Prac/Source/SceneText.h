#ifndef SCENETEXT_H
#define SCENETEXT_H

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

class SceneText : public Scene
{
	enum GEOMETRY_TYPE
	{
		GEO_AXES = 0,
		GEO_LEFT,
		GEO_LEFT2,
		GEO_RIGHT,
		GEO_RIGHT2,
		GEO_TOP,
		GEO_TOP2,
		GEO_BOTTOM,
		GEO_BOTTOM2,
		GEO_FRONT,
		GEO_FRONT2,
		GEO_BACK,
		GEO_BACK2,
		GEO_CHAR,
		GEO_DICE,
		GEO_CAR,
		GEO_CAR1,
		GEO_CAR2,
		GEO_CAR3,
		GEO_CAR4,
		GEO_TURNTABLE1,
		GEO_TURNTABLE2,
		GEO_TURNTABLE3,
		GEO_TURNTABLE4,
		GEO_PILLAR,
		GEO_PILLAR2,
		GEO_PILLAR3,
		GEO_PILLAR4,
		GEO_RACETRACK,
		GEO_RING,
		GEO_RING2,
		GEO_LIGHTSPHERE,
		GEO_LIGHTSPHERE2,
		GEO_FORTUNEWHEEL,
		GEO_TEXT,
		GEO_TEXTBG,
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

		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,

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

	Camera2 camera;

	ReplayRace replay[2];

	irrklang::ISoundEngine* SoundEngine;
	
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderObject(Mesh* mesh, Object meshObject, bool enableLight);
	void RenderSkybox();
	void VerticeUpdate(Mesh* mesh, Object meshObject);
	void RenderNPC();
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void CalculateFrameRate();
	void ShopUI(int carnum);
	void printIndicator();

	bool inrange = false;
	bool keyPressed;
	bool abletoPress;
	bool buy;
	bool optionselected[3];
	bool movedown = false;
	bool moveup = false;
	bool isSpinning;
	float rotationSpeed;

	int selection;
	double bounceTime;
	double elapsedTime;

	float rotation; // NPC AI
	double dotProduct;
	Vector3 modelView;
	Vector3 targetView;
	
	float currentTime;

	Acceleration objectA;
	Acceleration objectB;

	Player playerData;

	float ytranslate;
	float ytranslate2;
	float rotation1;
	float rotation2;
	float lightposx;
	float lightposz;
	float ringposx;
	float ringposz;

	bool scenecheck;
public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	virtual void RenderLeftScreen();
	virtual void RenderRightScreen();
	virtual bool SwitchScene();
	virtual bool skyboxcheck();
	virtual void Reset();
};

#endif