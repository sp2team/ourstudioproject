#ifndef SCENE_H
#define SCENE_H

class Scene
{
public:
	Scene() {}
	~Scene() {}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;
<<<<<<< HEAD
	virtual void RenderLeftScreen() = 0;
	virtual void RenderRightScreen() = 0;
	virtual int SwitchScene() = 0;
=======
	virtual bool skyboxcheck() = 0;
>>>>>>> models
};

#endif