#pragma once

#include <glm\vec3.hpp>
#include <glm\gtx\quaternion.hpp>

#include "Model.h"
#include "PhysicsThingy.h"

using namespace GraphicsLibrary;
using namespace PhysicsLibrary;

class Thingy
{
public:
	Thingy(Model* model);
	~Thingy();

	Model* GetModel();
private:
	Model* model;
	PhysicsThingy* physicsThingy;
};

