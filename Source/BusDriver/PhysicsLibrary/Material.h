#pragma once

#include <glm\vec3.hpp>
#include <glm\gtx\quaternion.hpp>

#include <PxPhysicsAPI.h>

#include "Physics.h"

using namespace physx;

namespace PhysicsLibrary
{
	class Material
	{
	public:
		Material(Physics* physics, float staticFriction, float dynamicFriction, float restitution);
		~Material();
	private:
		PxMaterial* material;
	};
}