#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <PxPhysicsAPI.h>
#include <vehicle\PxVehicleUtil.h>

#include "Physics.h"
#include "VehicleCreate.h"
#include "VehicleRaycast.h"
#include "VehicleTireFriction.h"

using namespace glm;
using namespace physx;

namespace PhysicsLibrary
{
	class Vehicle
	{
	public:
		Vehicle(Physics* physics);
		~Vehicle();

		void Update(float elapsedTime);

		vec3 GetPosition();

		quat GetRotation();
	private:
		Physics* physics;
		VehicleSceneQueryData*	gVehicleSceneQueryData = NULL;
		PxBatchQuery*			gBatchQuery = NULL;

		PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

		PxRigidStatic*			gGroundPlane = NULL;
		PxVehicleDrive4W*		gVehicle4W = NULL;
	};
}

