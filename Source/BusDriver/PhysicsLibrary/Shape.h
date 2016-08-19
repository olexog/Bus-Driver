#pragma once

#include <PxPhysicsAPI.h>

#include "Physics.h"

using namespace physx;

namespace PhysicsLibrary
{
	class Shape
	{
	public:
		Shape(Physics* physics, PxGeometry* geometry);
		Shape(Physics* physics, PxGeometry* geometry, PxVec3 position, PxQuat orientation);
		~Shape();

		void AddToActor(Physics* physics, PxRigidActor* actor);
	private:
		PxGeometry* geometry;
		PxVec3 position;
		PxQuat orientation;
	};
}