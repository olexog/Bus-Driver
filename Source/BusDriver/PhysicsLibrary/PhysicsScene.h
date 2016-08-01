#pragma once

#include <vector>

#include <PxPhysicsAPI.h>

#include "VehicleRaycast.h"

#include "Physics.h"
#include "StaticActor.h"
#include "DynamicActor.h"
#include "Vehicle.h"

using namespace std;
using namespace physx;

namespace PhysicsLibrary
{
	class PhysicsScene
	{
	public:
		PhysicsScene(Physics* physics);
		~PhysicsScene();

		void AddActor(StaticActor* actor);
		void AddActor(DynamicActor* actor);
		void AddActor(Vehicle* actor);

		void Simulate(float elapsedTime);
	private:
		PxScene* scene;
		PxDefaultCpuDispatcher* dispatcher;

		VehicleSceneQueryData* vehicleSceneQueryData;
		PxBatchQuery* batchQuery;

		vector<Vehicle*> vehicles;
	};
}
