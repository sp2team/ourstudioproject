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

	// Any new models/objects just add it in here
	// Object Car1;
	// Object Turntable;
	// Object Booth;
	// etc

	bool MeshCollision(Mesh* mesh1, Mesh* mesh2); // Returns a boolean to check for the collision between 2 meshes using stored vertices.
	ObjectManager();
	~ObjectManager();
};

#endif
