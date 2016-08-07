#include "Playground.h"

namespace PhysicsLibrary
{
	Playground::Playground(Physics* physics)
	{
		// create scene description
		PxSceneDesc sceneDesc(physics->GetPhysics()->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

		PxU32 numWorkers = 1;
		this->dispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
		sceneDesc.cpuDispatcher = this->dispatcher;
		sceneDesc.filterShader = VehicleFilterShader;

		this->scene = physics->GetPhysics()->createScene(sceneDesc);

		//Create the batched scene queries for the suspension raycasts.
		this->vehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, physics->GetAllocator());
		this->batchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *this->vehicleSceneQueryData, this->scene);
	}

	Playground::~Playground()
	{
		for (Actor* actor : this->actors)
		{
			delete actor;
		}
	}

	void Playground::AddActor(Actor* actor)
	{
		actor->AddToScene(this->scene);
		this->actors.push_back(actor);
	}

	void Playground::AddActor(DynamicActor* actor)
	{
		actor->AddToScene(this->scene);
	}

	void Playground::AddActor(Vehicle* actor)
	{
		actor->AddToScene(this->scene);
		this->vehicles.push_back(actor);
	}

	void Playground::Simulate(float elapsedTime)
	{
		PxRaycastQueryResult* raycastResults = this->vehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = this->vehicleSceneQueryData->getRaycastQueryResultBufferSize();

		for (Vehicle* vehicle : this->vehicles)
		{
			vehicle->UpdateInput(elapsedTime);
			vehicle->Update(elapsedTime, this->batchQuery, raycastResults, raycastResultsSize, this->scene->getGravity());
		}

		this->scene->simulate(elapsedTime);
		this->scene->fetchResults(true);
	}
}
