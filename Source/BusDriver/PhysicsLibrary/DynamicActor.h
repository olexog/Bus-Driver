#pragma once

#include <glm\vec3.hpp>
#include <glm\gtc\quaternion.hpp>

#include <PxPhysicsAPI.h>

#include "Physics.h"

using namespace glm;
using namespace physx;

namespace PhysicsLibrary
{
	class DynamicActor
	{
	public:
		DynamicActor(Physics* physics, vec3 halfExtends, vec3 position);
		~DynamicActor();

		void AddToScene(PxScene* scene);

		vec3 GetPosition();
		quat GetRotation();
	private:
		PxRigidDynamic* actor;
	};
}

