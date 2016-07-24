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

		void Simulate(float elapsedTime);

		PxPhysics* GetPhysics();
		PxMaterial* GetMaterial();
		PxCooking* GetCooking();
		PxAllocatorCallback& GetAllocator();
		PxScene* GetScene();

		void AddActor(PxActor &actor);

		PxFilterData obstacleSimFilterData;
	private:
		PxDefaultAllocator allocator;
		PxDefaultErrorCallback  errorCallback;

		PxFoundation* foundation;
		PxPhysics* physics;

		PxCooking* cooking;

		PxVisualDebuggerConnection* connection;

		PxDefaultCpuDispatcher* dispatcher;
		PxScene* scene;
		PxMaterial* material;

		PxRigidStatic* plane;
	};
}
