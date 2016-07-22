#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

namespace PhysicsLibrary
{
	class Physics
	{
	public:
		Physics();
		~Physics();
	private:
		PxDefaultAllocator		gAllocator;
		PxDefaultErrorCallback	gErrorCallback;

		PxFoundation*			gFoundation = NULL;
		PxPhysics*				gPhysics = NULL;

	};
}
