#pragma once

#include <vector>

#include <PxPhysicsAPI.h>

#include "VehicleRaycast.h"

#include "Physics.h"
#include "Actor.h"
#include "DynamicActor.h"
#include "Vehicle.h"

using namespace std;
using namespace physx;

namespace PhysicsLibrary
{
	class Playground
	{
	public:
		Playground(Physics* physics);
		~Playground();

		void AddActor(Actor* actor);
		void AddActor(DynamicActor* actor);
		void AddActor(Vehicle* actor);

		void Simulate(float elapsedTime);
	private:
		PxScene* scene;
		PxDefaultCpuDispatcher* dispatcher;

		VehicleSceneQueryData* vehicleSceneQueryData;
		PxBatchQuery* batchQuery;

		vector<Vehicle*> vehicles;
		vector<Actor*> actors;
		vector<DynamicActor*> dynamicActors;
	};
}
