#pragma once

#include <PxPhysicsAPI.h>

#include "VehicleFilterShader.h"

using namespace physx;

namespace PhysicsLibrary
{
	class Physics
	{
	public:
		Physics();
		~Physics();

		PxPhysics* GetPhysics();
		PxMaterial* GetMaterial();
		PxCooking* GetCooking();
		PxAllocatorCallback& GetAllocator();

		PxFilterData obstacleSimFilterData;
	private:
		PxDefaultAllocator allocator;
		PxDefaultErrorCallback  errorCallback;

		PxFoundation* foundation;
		PxPhysics* physics;

		PxCooking* cooking;

		PxVisualDebuggerConnection* connection;

		PxMaterial* material;

		PxRigidStatic* plane;
	};
}
