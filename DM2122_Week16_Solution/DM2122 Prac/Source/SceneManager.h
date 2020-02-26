#pragma once
#include "SceneText.h"

class SceneManager
{
private:
	Scene* scene;
	Scene* scene1;
	Scene* scene2;
	int switchScene;

public:
	SceneManager();
	~SceneManager();

	void Init();
	void RenderScreen(double time);
	void SwitchScreen();
	void Exit();
};

