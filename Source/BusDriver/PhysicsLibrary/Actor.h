#pragma once

#include <glm\vec3.hpp>
#include <glm\gtx\quaternion.hpp>

#include <PxPhysicsAPI.h>

#include "Physics.h"
#include "Body.h"

using namespace physx;

namespace PhysicsLibrary
{
	class Actor
	{
	public:
		Actor(Physics* physics, Body* body, vec3* position, quat* orientation);
		~Actor();

		void AddToScene(PxScene* scene);

		vector<vec3> GetPoints();
		vec3 GetPosition();
	private:
		Body* body;
		PxRigidActor* actor;
		vec3* position;
		quat* orientation;
	};
}