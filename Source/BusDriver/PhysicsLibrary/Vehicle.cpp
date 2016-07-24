#include "Vehicle.h"

namespace PhysicsLibrary
{
	VehicleDesc initVehicleDesc(PxMaterial* material)
	{
		//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
		//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
		//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
		const PxF32 chassisMass = 1500.0f;
		const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
		const PxVec3 chassisMOI
		((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
		const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

		//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
		//Moment of inertia is just the moment of inertia of a cylinder.
		const PxF32 wheelMass = 20.0f;
		const PxF32 wheelRadius = 0.5f;
		const PxF32 wheelWidth = 0.4f;
		const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
		const PxU32 nbWheels = 6;

		VehicleDesc vehicleDesc;
		vehicleDesc.chassisMass = chassisMass;
		vehicleDesc.chassisDims = chassisDims;
		vehicleDesc.chassisMOI = chassisMOI;
		vehicleDesc.chassisCMOffset = chassisCMOffset;
		vehicleDesc.chassisMaterial = material;
		vehicleDesc.wheelMass = wheelMass;
		vehicleDesc.wheelRadius = wheelRadius;
		vehicleDesc.wheelWidth = wheelWidth;
		vehicleDesc.wheelMOI = wheelMOI;
		vehicleDesc.numWheels = nbWheels;
		vehicleDesc.wheelMaterial = material;
		return vehicleDesc;
	}

	Vehicle::Vehicle(Physics* physics)
	{
		this->physics = physics;
		//Create the batched scene queries for the suspension raycasts.
		gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, physics->GetAllocator());
		gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, physics->GetScene());

		//Create the friction table for each combination of tire and surface type.
		gFrictionPairs = createFrictionPairs(physics->GetMaterial());

		//Create a plane to drive on.
		gGroundPlane = createDrivablePlane(physics->GetMaterial(), physics->GetPhysics());
		physics->AddActor(*gGroundPlane);

		VehicleDesc vehicleDesc = initVehicleDesc(physics->GetMaterial());
		gVehicle4W = createVehicle4W(vehicleDesc, physics->GetPhysics(), physics->GetCooking());
		PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
		gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
		physics->AddActor(*gVehicle4W->getRigidDynamicActor());

		gVehicle4W->setToRestState();
		gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		gVehicle4W->mDriveDynData.setUseAutoGears(true);
	}

	Vehicle::~Vehicle()
	{
	}

	void Vehicle::Update(float elapsedTime)
	{
		//Raycasts.
		PxVehicleWheels* vehicles[1] = { gVehicle4W };
		PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = gVehicleSceneQueryData->getRaycastQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

		//Vehicle update.
		const PxVec3 grav = this->physics->GetScene()->getGravity();
		PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		PxVehicleWheelQueryResult vehicleQueryResults[1] = { { wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels() } };
		PxVehicleUpdates(elapsedTime, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);
	}

	vec3 Vehicle::GetPosition()
	{
		PxVec3 position = this->gVehicle4W->getRigidDynamicActor()->getGlobalPose().p;
		return vec3(position.x, position.y, position.z);
	}

	quat Vehicle::GetRotation()
	{
		PxQuat rotation = this->gVehicle4W->getRigidDynamicActor()->getGlobalPose().q;
		return quat(rotation.w, rotation.x, rotation.y, rotation.z);
	}
}
