#include "ObjectManager.h"

ObjectManager ObjectList;

bool ObjectManager::PointToMeshCollision(Vector3 point, Mesh* mesh) // Checks whether a vector is within the mesh's bounding box
{
	if (point.x >= mesh->minX && point.x <= mesh->maxX &&
		point.z >= mesh->minZ && point.z <= mesh->maxZ)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ObjectManager::MeshCollision(Mesh* mesh1, Mesh* mesh2) // Checks whether a mesh intersects with another mesh's bounding box
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
