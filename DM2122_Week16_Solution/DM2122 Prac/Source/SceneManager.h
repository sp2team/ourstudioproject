#pragma once
#include "SceneText.h"

class SceneManager
{
private:
	Scene* scene;
	int switchScene;

public:
	SceneManager();
	~SceneManager();

	void Init();
	void RenderScreen(double time);
	void SwitchScreen();
	void Exit();
};

