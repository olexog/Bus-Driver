#include "Physics.h"

namespace PhysicsLibrary
{
	Physics::Physics()
	{
		// create the basic PhysX instances
		this->foundation = PxCreateFoundation(PX_PHYSICS_VERSION, this->allocator, this->errorCallback);
		PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(this->foundation);
		this->physics = PxCreatePhysics(PX_PHYSICS_VERSION, *this->foundation, PxTolerancesScale(), true, profileZoneManager);

		// connect to the PVD if it is running
		if (this->physics->getPvdConnectionManager())
		{
			this->physics->getVisualDebugger()->setVisualizeConstraints(true);
			this->physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
			this->physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
			this->connection = PxVisualDebuggerExt::createConnection(this->physics->getPvdConnectionManager(), "127.0.0.1", 5425, 10);
		}

		// initialize the vehicle SDK
		PxInitVehicleSDK(*this->physics);
		PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
		PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

		this->drivableObstacleSimulationFilterData.word0 = COLLISION_FLAG_GROUND;
		this->drivableObstacleSimulationFilterData.word1 = COLLISION_FLAG_GROUND_AGAINST;
		this->drivableObstacleQueryFilterData.word3 = (PxU32)DRIVABLE_SURFACE;

		this->wheelSimulationFilterData.word0 = COLLISION_FLAG_WHEEL;
		this->wheelSimulationFilterData.word1 = COLLISION_FLAG_WHEEL_AGAINST;
		this->wheelQueryFilterData.word3 = UNDRIVABLE_SURFACE;

		this->chassisSimulationFilterData.word0 = COLLISION_FLAG_CHASSIS;
		this->chassisSimulationFilterData.word1 = COLLISION_FLAG_CHASSIS_AGAINST;
		this->chassisQueryFilterData.word3 = UNDRIVABLE_SURFACE;

		obstacleSimFilterData.word0 = COLLISION_FLAG_GROUND;
		obstacleSimFilterData.word1 = COLLISION_FLAG_GROUND_AGAINST;

		this->material = this->physics->createMaterial(0.5f, 0.5f, 0.6f);

		this->cooking = PxCreateCooking(PX_PHYSICS_VERSION, *this->foundation, PxCookingParams(PxTolerancesScale()));

		//this->plane = PxCreatePlane(*this->physics, PxPlane(0, 1, 0, 0), *this->material);
		//this->scene->addActor(*this->plane);
	}

	Physics::~Physics()
	{
		PxCloseVehicleSDK();

		PxProfileZoneManager* profileZoneManager = this->physics->getProfileZoneManager();
		if (this->connection != NULL)
			this->connection->release();
		this->physics->release();
		profileZoneManager->release();
		this->foundation->release();
		// TODO: release parts
	}

	PxPhysics* Physics::GetPhysics()
	{
		return this->physics;
	}

	PxMaterial* Physics::GetMaterial()
	{
		return this->material;
	}

	PxCooking* Physics::GetCooking()
	{
		return this->cooking;
	}

	PxAllocatorCallback& Physics::GetAllocator()
	{
		return this->allocator;
	}

	PxFilterData Physics::GetDrivableObstacleSimulationFilterData()
	{
		return this->drivableObstacleSimulationFilterData;
	}

	PxFilterData Physics::GetDrivableObstacleQueryFilterData()
	{
		return this->drivableObstacleQueryFilterData;
	}
	PxFilterData Physics::GetWheelSimulationFilterData()
	{
		return this->wheelSimulationFilterData;
	}
	PxFilterData Physics::GetWheelQueryFilterData()
	{
		return this->wheelQueryFilterData;
	}
	PxFilterData Physics::GetChassisSimulationFilterData()
	{
		return this->chassisSimulationFilterData;
	}
	PxFilterData Physics::GetChassisQueryFilterData()
	{
		return this->chassisQueryFilterData;
	}
}
