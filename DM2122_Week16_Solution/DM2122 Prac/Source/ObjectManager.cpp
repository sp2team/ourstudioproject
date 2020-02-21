#include "ObjectManager.h"

ObjectManager ObjectList;

bool ObjectManager::MeshCollision(Mesh* mesh1, Mesh* mesh2)
{
	if (mesh1->maxX >= mesh2->minX && mesh1->minX <= mesh2->maxX &&
		mesh1->maxY >= mesh2->minY && mesh1->minY <= mesh2->maxY &&
		mesh1->maxZ >= mesh2->minZ && mesh1->minZ <= mesh2->maxZ)
	{
		return true;
	}
	else
	{
		return false;
	}
}

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
}
