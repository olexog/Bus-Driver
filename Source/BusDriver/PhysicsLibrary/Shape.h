#pragma once

#include <PxPhysicsAPI.h>

#include "Physics.h"

using namespace physx;

namespace PhysicsLibrary
{
	class Shape
	{
	public:
		enum Type
		{
			CHASSIS,
			WHEEL,
			OBSTACLE
		};

		Shape(Physics* physics, PxGeometry* geometry);
		Shape(Physics* physics, PxGeometry* geometry, PxVec3 position, PxQuat orientation, Type type = OBSTACLE);
		~Shape();

		void AddToActor(Physics* physics, PxRigidActor* actor, PxShape* &shape);

		void SetType(Type type);
	private:
		PxGeometry* geometry;
		PxVec3 position;
		PxQuat orientation;
		Type type;
	};
}