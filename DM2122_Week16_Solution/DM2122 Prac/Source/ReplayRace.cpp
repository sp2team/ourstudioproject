#include "ReplayRace.h"

ReplayRace replay[2];

ReplayRace::ReplayRace()
{
	first = last = newnode = NULL;
	count = 0;
}

ReplayRace::~ReplayRace()
{
}

void ReplayRace::resetReplay()
{
	//removes the previous replay to clear up memory
	for (int i = 0; i < count; ++i)
	{
		newnode = first;
		first = first->nextFrame;
		newnode = NULL;
	}

	//resets the pointer to NULL
	first = last = newnode = NULL;
	count = 0;
}

void ReplayRace::saveFrame(Object data)
{
	//records car position data
	newnode = new CNode;
	newnode->data = data;

	//links the pointers together
	if (first == NULL)
	{
		newnode->nextFrame = NULL;
		last = first = newnode;
	}
	else
	{
		last->nextFrame = newnode;
		last = newnode;
	}
	count++;
}

Object ReplayRace::Replay()
{
	if (newnode == NULL)
	{
		Object null;
		return null;
	}

	//sets the newnode pointer to the next frame, in order to return it
	if (newnode == last)
		newnode = first;
	else
		newnode = newnode->nextFrame;

	//returns the car data for the current frame
	return newnode->data;
}
