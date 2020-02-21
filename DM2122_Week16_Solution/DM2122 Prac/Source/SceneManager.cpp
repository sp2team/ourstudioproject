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
	switchScene = 0;
	scene = new SceneText;
	scene->Init();
}

void SceneManager::RenderScreen(double time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (switchScene == 0)
	{
		//Render Single Screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 1920, 1000);
		scene->Render();
		scene->Update(time);
	}
	else if (switchScene == 1)
	{
		//Render Split Screen
		glViewport(1920 / 2, 0, 1920 / 2, 1000);
		scene->RenderRightScreen();
		scene->Update(time);

		glViewport(0, 0, 1920 / 2, 1000);
		scene->RenderLeftScreen();
		scene->Update(time);
	}
}

void SceneManager::SwitchScreen()
{
	switchScene = scene->SwitchScene();
}

void SceneManager::Exit()
{
	scene->Exit();
}