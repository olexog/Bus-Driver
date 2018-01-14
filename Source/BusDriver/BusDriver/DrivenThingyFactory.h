#pragma once

#include "Physics.h"
#include "Playground.h"
#include "Scene.h"
#include "DrivenThingy.h"
#include "ModelReader.h"

using namespace PhysicsLibrary;
using namespace GraphicsLibrary;

class DrivenThingyFactory
{
public:
	DrivenThingyFactory(Physics* physics, Playground* playground, Scene* scene);
	~DrivenThingyFactory();

	DrivenThingy* Create(int id, PxVec3 startingPosition);
private:
	Physics* physics;
	Playground* playgound;
	Scene* scene;

	vector<vector<vec3>> wheelVertices;
	vector<Model*> wheelModels;
	vector<Model*> chassisModels;
	vector<float> chassisWidths;
	vector<float> chassisHeights;
	vector<float> chassisLengths;
	vector<float> chassisMasses;
	vector<float> wheelMasses;
	vector<float> wheelRadiuses;
	vector<float> wheelWidths;
	vector<float> prolapses;
	vector<float> frontAxisOffsets;
	vector<float> rearAxisOffsets;
};

