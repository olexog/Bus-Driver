#pragma once

#include <vector>

#include <PxPhysicsAPI.h>

#include <glm\vec3.hpp>
#include <glm\gtx\quaternion.hpp>

#include "Physics.h"

using namespace std;
using namespace physx;
using namespace glm;

namespace PhysicsLibrary
{
	class StaticActor
	{
	public:
		StaticActor(Physics* physics, vector<vec3> points, vec3 position, quat orientation);
		~StaticActor();

		void AddToScene(PxScene* scene);
	private:
		PxRigidStatic* actor;
	};
}

