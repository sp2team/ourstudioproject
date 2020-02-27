#pragma once
#include "SceneText.h"
#include "SceneRace.h"

class SceneManager
{
private:
	Scene* scene;
	Scene* scene1;
	Scene* scene2;

public:
	SceneManager();
	~SceneManager();

	void Init();
	void RenderScreen(double time);
	void SwitchScreen();
	void Exit();
};

