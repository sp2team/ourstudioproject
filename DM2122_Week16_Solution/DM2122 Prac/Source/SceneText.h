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
#pragma comment(lib, "irrKlang.lib")

class SceneText : public Scene
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
		GEO_CHAR,
<<<<<<< HEAD
		GEO_DICE,
		GEO_CAR,
=======
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
>>>>>>> models
		GEO_LIGHTSPHERE,
		GEO_LIGHTSPHERE2,
		GEO_TEXT,
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
<<<<<<< HEAD

	Light light[1];
=======
	Light light[2];
>>>>>>> models

	Vector3 playerPos;

	Camera2 camera[2];

	irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();

	int screen;
	
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderObject(Mesh* mesh, Object meshObject, bool enableLight);
	void RenderSkybox();
	void VerticeUpdate(Mesh* mesh, Object meshObject);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderLeftScreen();
	void RenderRightScreen();
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

<<<<<<< HEAD
	int selection;
	double bounceTime;
	double elapsedTime;

	bool isAcceleratingA;
	bool isDeceleratingA;
	float accelerationA;
	
	bool isAcceleratingB;
	bool isDeceleratingB;
	float accelerationB;
	
	float currentTime;

	Acceleration objectA;
	Acceleration objectB;

	Cars cars[4];

	Player player1;
	Player player2;
=======
	float ytranslate;
	float ytranslate2;
	float rotation1;
	float rotation2;
	float lightposx;
	float lightposz;
	float ringposx;
	float ringposz;

	bool scenecheck;
>>>>>>> models

public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
<<<<<<< HEAD
	int SwitchScene();
	
=======
	virtual bool skyboxcheck();
>>>>>>> models
};

#endif