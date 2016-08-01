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

		void SetToRestState();

		void Update(float elapsedTime, PxBatchQuery* batchQuery, PxRaycastQueryResult* raycastResults, PxU32 raycastResultsSize, PxVec3 gravity);
		void UpdateInput(float elapsedTime);

		void Accelerate(float measure);
		void Brake(float measure);
		void Turn(float measure);
		void Handbrake(float measure);

		void AddToScene(PxScene* scene);

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

		PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

		PxRigidStatic*			gGroundPlane = NULL;
		PxVehicleDrive4W*		gVehicle4W = NULL;

		PxVehicleDrive4WRawInputData gVehicleInputData;

		vector<PxShape*> wheels;
		vector<PxShape*> chassis;
	};
}

