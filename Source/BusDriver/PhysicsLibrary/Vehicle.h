#pragma once

#include <vector>
#include <iostream>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <PxPhysicsAPI.h>
#include <vehicle\PxVehicleUtil.h>

#include "Physics.h"
#include "VehicleCreate.h"
#include "VehicleRaycast.h"
#include "VehicleTireFriction.h"

using namespace std;
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

		void Accelerate(float measure);
		void Brake(float measure);
		void Turn(float measure);
		void Handbrake(float measure);

		vector<vec3> GetShape();

		vec3 GetPosition();

		quat GetRotation();

		vector<vector<vec3>> GetWheelVertices();
		vector<vec3> GetWheelPositions();
		vector<quat> GetWheelRotations();

		vector<vec3> GetChassisVertices();
		vec3 GetChassisPosition();
		quat GetChassisRotation();
	private:
		Physics* physics;
		VehicleSceneQueryData*	gVehicleSceneQueryData = NULL;
		PxBatchQuery*			gBatchQuery = NULL;

		PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

		PxRigidStatic*			gGroundPlane = NULL;
		PxVehicleDrive4W*		gVehicle4W = NULL;

		PxVehicleDrive4WRawInputData gVehicleInputData;

		vector<PxShape*> wheels;
		vector<PxShape*> chassis;
	};
}

