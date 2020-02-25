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
	scene1 = new SceneText;
	scene2 = new SceneText2;
	scene = scene1;
	scene1->Init();
	scene2->Init();
}

void SceneManager::RenderScreen(double time)
{
	//Clear color & depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//if (switchScene == 0)
	//{
	//	//Render Single Screen
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glViewport(0, 0, 1920, 1000);
	//	scene->Render();
	//	scene->Update(time);
	//}
	//else if (switchScene == 1)
	//{
		//Render Split Screen
		glViewport(1920 / 2, 0, 1920 / 2, 1000);
		scene1->RenderRightScreen();
		scene->Update(time);

		glViewport(0, 0, 1920 / 2, 1000);
		scene2->RenderLeftScreen();
		scene->Update(time);
	//}
}

void SceneManager::SwitchScreen()
{
	switchScene = scene->SwitchScene();
	if (switchScene == 0)
		scene = scene1;
	else if (switchScene == 1)
		scene = scene2;
}

void SceneManager::Exit()
{
	scene1->Exit();
	scene2->Exit();
}