#pragma once

#include <glm\vec3.hpp>
#include <glm\gtc\quaternion.hpp>

#include <PxPhysicsAPI.h>

#include "Physics.h"
#include "Body.h"

using namespace glm;
using namespace physx;

namespace PhysicsLibrary
{
	class DynamicActor
	{
	public:
		DynamicActor(Physics* physics, Body* body, vec3* position, quat* orientation);
		~DynamicActor();

		void AddToScene(PxScene* scene);

		void Update();
	protected:
		Body* body;
		PxRigidDynamic* actor;
		vec3* position;
		quat* orientation;
	};
}

