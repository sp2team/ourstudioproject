#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H
#include "Object.h"
#include "Mesh.h"

class ObjectManager
{
public:
	Object LightSphere;
	Object Character;
	Object Character2;

	bool MeshCollision(Mesh* mesh1, Mesh* mesh2);
	ObjectManager();
	~ObjectManager();
};

#endif
