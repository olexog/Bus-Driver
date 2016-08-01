#include "PhysicsScene.h"

namespace PhysicsLibrary
{
	PhysicsScene::PhysicsScene(Physics* physics)
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

	PhysicsScene::~PhysicsScene()
	{

	}

	void PhysicsScene::AddActor(StaticActor* actor)
	{
		actor->AddToScene(this->scene);
	}

	void PhysicsScene::AddActor(DynamicActor* actor)
	{
		actor->AddToScene(this->scene);
	}

	void PhysicsScene::AddActor(Vehicle* actor)
	{
		actor->AddToScene(this->scene);
		this->vehicles.push_back(actor);
	}

	void PhysicsScene::Simulate(float elapsedTime)
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
