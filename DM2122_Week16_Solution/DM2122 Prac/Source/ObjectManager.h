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
	Object fortuneWheel;
	Object Car1;
	Object Car2;
	Object Car3;
	Object Car4;
	Object Racetrack;
	Object leftwall;
	Object rightwall;
	Object roof;
	Object floor;
	Object frontwall;
	Object backwall;
	Object leftwall2;
	Object rightwall2;
	Object roof2;
	Object frontwall2;
	Object backwall2;
	Object pillar1;
	Object pillar2;
	Object pillar3;
	Object pillar4;
	Object ring1;
	Object ring2;
	Object turntable1;
	Object turntable2;
	Object turntable3;
	Object turntable4;
	Object newcar1;
	Object newcar2;
	Object newcar3;
	Object newcar4;

	// Scene 2
	Object obstacle1;
	Object obstacle2;
	Object obstacle3;
	Object obstacle4;
	Object boundary;


	// Any new models/objects just add it in here
	// Object Car1;
	// Object Turntable;
	// Object Booth;
	// etc

	bool PointToMeshCollision(Vector3 point, Mesh* mesh); // Returns a boolean to check if a point is inside a mesh's bounding box.
	bool MeshCollision(Mesh* mesh1, Mesh* mesh2); // Returns a boolean to check for the collision between 2 meshes using stored vertices.
	ObjectManager();
	~ObjectManager();
};

#endif
