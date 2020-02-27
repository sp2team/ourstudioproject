#pragma once
#include "ObjectManager.h"

class ReplayRace
{
private:
	class CNode
	{
	public:
		Object data;
		CNode* nextFrame;
	};
	CNode* first;
	CNode* last;
	CNode* newnode;

	int count;
public:
	ReplayRace();
	~ReplayRace();

	void resetReplay();
	void saveFrame(Object data);
	Object Replay();
};
