#pragma once

#include <string>

#include "GraphicsUtility.h"
#include "Thingy.h"
#include "ModelReader.h"
#include "Physics.h"
#include "BodyReader.h"
#include "PhysicsUtility.h"

using namespace std;
using namespace GraphicsLibrary;
using namespace PhysicsLibrary;

class ThingyReader
{
public:
	static Thingy* Read(Physics* physics, string fileName);
};

