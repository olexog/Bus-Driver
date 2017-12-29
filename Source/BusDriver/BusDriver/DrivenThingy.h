#pragma once

#include <vector>

#include "Scene.h"
#include "Vehicle.h"
#include "Thingy.h"
#include "Physics.h"
#include "PhysicsUtility.h"
#include "Playground.h"

using namespace glm;
using namespace GraphicsLibrary;
using namespace PhysicsLibrary;

class DrivenThingy
{
public:
	DrivenThingy(vector<vec3> wheelVertices, Physics* physics, float chassisWidth, float chassisHeight, float chassisLength,
		Model* wheelModel, Model* chassisModel, float chassisMass, float wheelMass, float wheelRadius, float wheelWidth,
		float prolapse, float frontAxisOffset, float rearAxisOffset,
		PxVec3 startingPosition, Playground* playground);
	~DrivenThingy();
	void AddToScene(Scene* scene);

	void Update();

	Vehicle* GetVehicle();
private:
	vector<vec3*> wheelPositions = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelOrientations = { new quat(), new quat(), new quat(), new quat() };
	vector<vec3*> wheelLocalPositions = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelLocalOrientations = { new quat(), new quat(), new quat(0.7071f, 0.7071f, 0.0f, 0.0f), new quat(0.7071f, 0.7071f, 0.0f, 0.0f) };
	vector<Shape*> wheels;
	vec3* chassisPosition = new vec3();
	quat* chassisOrientation = new quat();
	vec3* chassisLocalPosition = new vec3(0.0f, 0.0f, 0.0f);
	quat* chassisLocalOrientation = new quat();
	PxConvexMesh* chassisMesh;
	PxConvexMeshGeometry* chassisGeometry;
	Shape* chassis;

	Model* wheelModel;
	Model* chassisModel;

	PxF32 chassisMass;
	PxVec3 chassisDims;
	PxVec3 chassisMOI;
	PxVec3 chassisCMOffset;

	PxF32 wheelMass;
	PxF32 wheelRadius;
	PxF32 wheelWidth;
	PxF32 wheelMOI;

	PxF32 wheelBase;
	PxF32 prolapse;

	PxF32 rearAxisOffset;
	PxF32 frontAxisOffset;

	Vehicle* vehicle;
};

