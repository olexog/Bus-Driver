#pragma once

#include <vector>

#include <glm\vec3.hpp>

#include <PxPhysicsAPI.h>

#include "Physics.h"
#include "PositionedGeometry.h"

using namespace std;
using namespace glm;
using namespace physx;

namespace PhysicsLibrary
{
	class PhysicsThingy
	{
	public:
		PhysicsThingy(Physics* physics, vec3 position, vector<PositionedGeometry> positionedGeometries);
		~PhysicsThingy();
	private:
		PxRigidActor* actor;
	};
}