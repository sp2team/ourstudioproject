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
		GEO_DICE,
		GEO_CAR,
		GEO_LIGHTSPHERE,
		GEO_TEXT,
		GEO_DECAL,
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

	Light light[1];

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
	void RenderAnimation(Mesh* mesh);
	void CalculateFrameRate();


	bool isAcceleratingA;
	bool isDeceleratingA;
	float accelerationA;
	
	bool isAcceleratingB;
	bool isDeceleratingB;
	float accelerationB;
	
	float currentTime;

	Acceleration objectA;
	Acceleration objectB;

	int decal;
	float decalLastTime;
public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	int SwitchScene();
	
};

#endif