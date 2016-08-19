#pragma once

#include <PxPhysicsAPI.h>

#include "VehicleFilterShader.h"
#include "VehicleRaycast.h"

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
		PxFilterData GetDrivableObstacleSimulationFilterData();
		PxFilterData GetDrivableObstacleQueryFilterData();

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

		PxFilterData drivableObstacleSimulationFilterData;
		PxFilterData drivableObstacleQueryFilterData;
	};
}
