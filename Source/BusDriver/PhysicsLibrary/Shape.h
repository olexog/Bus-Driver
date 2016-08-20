#pragma once

#include <PxPhysicsAPI.h>

#include <glm\vec3.hpp>
#include <glm\gtx\quaternion.hpp>

#include "Physics.h"

using namespace physx;
using namespace glm;

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
		Shape(Physics* physics, PxGeometry* geometry, vec3* position, quat* orientation, Type type = OBSTACLE);
		~Shape();

		void AddToActor(Physics* physics, PxRigidActor* actor, PxShape* &shape);
		void Update();
		void SetType(Type type);
		void SetPosePointer(vec3* position, quat* orientation);
	private:
		PxGeometry* geometry;
		PxShape* shape;
		vec3* position;
		quat* orientation;
		Type type;
	};
}