#pragma once

#include <vector>
#include <iostream>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <PxPhysicsAPI.h>
#include <vehicle\PxVehicleUtil.h>

#include "Physics.h"
#include "DynamicActor.h"
#include "Shape.h"
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
		Vehicle(Physics* physics, Shape* chassis, vector<Shape*> wheels, PxTransform startingPose,
			PxF32 chassisMass, PxVec3 chassisDimensions, PxVec3 chassisMOI, PxMaterial* chassisMaterial, PxVec3 chassisCMOffset,
			PxF32 wheelMass, PxF32 wheelRadius, PxF32 wheelWidth, PxF32 wheelMOI, PxMaterial* wheelMaterial, PxU32 wheelNumber,
			PxF32 frontAxisOffset, PxF32 rearAxisOffset, PxF32 prolapse);
		~Vehicle();

		void SetToRestState();

		void Update(float elapsedTime, PxBatchQuery* batchQuery, PxRaycastQueryResult* raycastResults, PxU32 raycastResultsSize, PxVec3 gravity);
		void UpdateInput(float elapsedTime);

		void Accelerate(float measure);
		void Brake(float measure);
		void Turn(float measure);
		void Handbrake(float measure);
		void SetForwardGear();
		void SetReverseGear();

		void AddToScene(PxScene* scene);

		vec3 GetPosition();
		quat GetRotation();
	private:
		Physics* physics;

		PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

		PxRigidStatic*			gGroundPlane = NULL;
		PxVehicleDrive4W*		gVehicle4W = NULL;

		PxVehicleDrive4WRawInputData gVehicleInputData;

		vector<Shape*> wheelShapes;
	};
}

