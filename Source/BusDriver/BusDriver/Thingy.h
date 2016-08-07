#pragma once

#include "Model.h"
#include "Body.h"

using namespace GraphicsLibrary;
using namespace PhysicsLibrary;

class Thingy
{
public:
	Thingy(Model* model, Body* body);
	~Thingy();

	Model* GetModel();
	Body* GetBody();
private:
	Model* model;
	Body* body;
};

