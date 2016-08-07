#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "Physics.h"
#include "Shape.h"
#include "Body.h"

using namespace std;

namespace PhysicsLibrary
{
	///<summary>This class is to read the contents of a physics thingy file.</summary>
	class BodyReader
	{
	public:
		///<summary>Reads the contents of the given physics thingy file and returns a vector of positioned geometries.</summary>
		static Body* Read(Physics* physics, string fileName);
	};
}