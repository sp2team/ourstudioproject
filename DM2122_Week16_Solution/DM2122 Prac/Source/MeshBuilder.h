#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "ObjectManager.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateBoundingBox(const std::string& meshName, Vector3 vertice[8]);
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string &meshName, Color color, float lengthX, float lengthY, Object meshObject);
	static Mesh* GenerateCuboid(const std::string& meshName, Color color, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateRing(const std::string& meshName, Color color, unsigned numSlice, float outerR, float innerR);
	static Mesh* GenerateSphere(const std::string& meshName, Color color, unsigned numStack, unsigned numSlice, float radius, Object meshObject);
	static Mesh* GenerateCone(const std::string& meshName, Color color, unsigned numSlice, float radius, float height);
	static Mesh* GenerateCylinder(const std::string& meshName, Color color, unsigned numStack, unsigned numSlice, float radius, float height);
	static Mesh* GenerateTorus(const std::string& meshName, Color color, unsigned numStack, unsigned numSlice, float outerR, float innerR);
	static Mesh* GenerateOBJ(const std::string& meshname, const std::string& file_path, Object meshObject);
	static Mesh* GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol);

};

#endif