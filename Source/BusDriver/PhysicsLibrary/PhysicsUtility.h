#pragma once

#include <vector>

#include "Physics.h"
#include "Body.h"
#include "Shape.h"

#include <glm\vec3.hpp>

using namespace physx;
using namespace std;
using namespace glm;

namespace PhysicsLibrary
{
	class PhysicsUtility
	{
	public:
		static Body* BodyFromTriangles(vector<vec3> vertices, Physics* physics);
		static Shape* ShapeFromConvexTriangles(vector<vec3> vertices, Physics* physics);
	};
}

