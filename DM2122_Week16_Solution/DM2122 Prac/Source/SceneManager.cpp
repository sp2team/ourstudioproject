#include "SceneManager.h"

#include "Application.h"
#include <GLFW/glfw3.h>

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	scene1 = new SceneText;
	scene2 = new SceneRace;
	scene = scene1;
	scene1->Init();
	scene2->Init();
}

void SceneManager::RenderScreen(double time)
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (scene == scene1)
	{
		//Render Single Screen
		glViewport(0, 0, 1920, 1000);
		scene1->Render();
		scene->Update(time);
	}
	else if (scene == scene2)
	{
		//Render Split Screen
		glViewport(1920 / 2, 0, 1920 / 2, 1000);
		scene2->RenderRightScreen();
		scene->Update(time);

		glViewport(0, 0, 1920 / 2, 1000);
		scene2->RenderLeftScreen();
		scene->Update(time);
	}
}

void SceneManager::SwitchScreen()
{
	if (scene->SwitchScene() == true)
	{
		if (scene == scene1)
		{
			scene = scene2;
			scene1->Reset();
			scene2->Reset();
		}
		else
		{
			scene1->Init();
			scene = scene1;
			scene2->Reset();
		}
	}
}

void SceneManager::Exit()
{
	scene1->Exit();
	scene2->Exit();
}