#pragma once

#include <PxPhysicsAPI.h>
#include <vehicle\PxVehicleUtil.h>

#include "Physics.h"
#include "VehicleCreate.h"
#include "VehicleRaycast.h"
#include "VehicleTireFriction.h"

using namespace physx;

namespace PhysicsLibrary
{
	class Vehicle
	{
	public:
		Vehicle(Physics* physics);
		~Vehicle();

		void Update(float elapsedTime);
	private:
		Physics* physics;
		VehicleSceneQueryData*	gVehicleSceneQueryData = NULL;
		PxBatchQuery*			gBatchQuery = NULL;

		PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

		PxRigidStatic*			gGroundPlane = NULL;
		PxVehicleDrive4W*		gVehicle4W = NULL;
	};
}

