#include "Vehicle.h"

namespace PhysicsLibrary
{
	bool					gIsVehicleInAir = true;

	PxF32 gSteerVsForwardSpeedData[2 * 8] =
	{
		0.0f,		0.75f,
		5.0f,		0.75f,
		30.0f,		0.125f,
		120.0f,		0.1f,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32
	};
	PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

	PxVehicleKeySmoothingData gKeySmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxVehiclePadSmoothingData gPadSmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	VehicleDesc initVehicleDesc(PxMaterial* material)
	{
		//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
		//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
		//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
		const PxF32 chassisMass = 1500.0f;
		const PxVec3 chassisDims(2.5f, 3.4f, 11.0f);
		const PxVec3 chassisMOI
		((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
		const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

		//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
		//Moment of inertia is just the moment of inertia of a cylinder.
		const PxF32 wheelMass = 20.0f;
		const PxF32 wheelRadius = 0.704f;
		const PxF32 wheelWidth = 0.748f;
		const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
		const PxU32 nbWheels = 4;

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
		gVehicle4W = createVehicle4W(vehicleDesc, wheels, chassis, physics->GetPhysics(), physics->GetCooking());
		PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxPi, PxVec3(0, 1, 0)));
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
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, elapsedTime, gIsVehicleInAir, *gVehicle4W);

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

		//Work out if the vehicle is in the air.
		gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

		std::cout << this->gVehicle4W->getRigidDynamicActor()->getLinearVelocity().magnitude() << endl;
	}

	void Vehicle::Accelerate(float measure)
	{
		gVehicleInputData.setAnalogAccel(measure);
	}

	void Vehicle::Brake(float measure)
	{
		gVehicleInputData.setAnalogBrake(measure);
	}

	// right+ left-
	void Vehicle::Turn(float measure)
	{
		this->gVehicleInputData.setAnalogSteer(measure);
	}

	vector<vec3> Vehicle::GetShape()
	{
		vector<vec3> result;

		for (PxShape* wheel : this->wheels)
		{
			PxTransform pose = wheel->getLocalPose();

			PxGeometryHolder geometry = wheel->getGeometry();

			switch (geometry.getType())
			{
			case PxGeometryType::eCONVEXMESH:
			{
				//Compute triangles for each polygon.
				const PxVec3 scale = geometry.convexMesh().scale.scale;
				PxConvexMesh* mesh = geometry.convexMesh().convexMesh;
				const PxU32 nbPolys = mesh->getNbPolygons();
				const PxU8* polygons = mesh->getIndexBuffer();
				const PxVec3* verts = mesh->getVertices();
				PxU32 nbVerts = mesh->getNbVertices();
				PX_UNUSED(nbVerts);

				PxVec3* vertices = new PxVec3[nbVerts];

				for (PxU32 i = 0; i < nbVerts; i++)
				{
					vertices[i] = pose.transform(verts[i]);
				}

				PxU32 numTotalTriangles = 0;
				for (PxU32 i = 0; i < nbPolys; i++)
				{
					PxHullPolygon data;
					mesh->getPolygonData(i, data);

					const PxU32 nbTris = data.mNbVerts - 2;
					const PxU8 vref0 = polygons[data.mIndexBase + 0];
					PX_ASSERT(vref0 < nbVerts);
					for (PxU32 j = 0; j < nbTris; j++)
					{
						const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
						const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

						result.push_back(vec3(vertices[vref0].x, vertices[vref0].y, vertices[vref0].z) * vec3(scale.x, scale.y, scale.z));
						result.push_back(vec3(vertices[vref1].x, vertices[vref1].y, vertices[vref1].z) * vec3(scale.x, scale.y, scale.z));
						result.push_back(vec3(vertices[vref2].x, vertices[vref2].y, vertices[vref2].z) * vec3(scale.x, scale.y, scale.z));
					}
				}
			} break;
			}
		}

		for (PxShape* chassis : this->chassis)
		{
			PxTransform pose = chassis->getLocalPose();

			PxGeometryHolder geometry = chassis->getGeometry();

			switch (geometry.getType())
			{
			case PxGeometryType::eCONVEXMESH:
			{
				//Compute triangles for each polygon.
				const PxVec3 scale = geometry.convexMesh().scale.scale;
				PxConvexMesh* mesh = geometry.convexMesh().convexMesh;
				const PxU32 nbPolys = mesh->getNbPolygons();
				const PxU8* polygons = mesh->getIndexBuffer();
				const PxVec3* verts = mesh->getVertices();
				PxU32 nbVerts = mesh->getNbVertices();
				PX_UNUSED(nbVerts);

				PxVec3* vertices = new PxVec3[nbVerts];

				for (PxU32 i = 0; i < nbVerts; i++)
				{
					vertices[i] = pose.transform(verts[i]);
				}

				PxU32 numTotalTriangles = 0;
				for (PxU32 i = 0; i < nbPolys; i++)
				{
					PxHullPolygon data;
					mesh->getPolygonData(i, data);

					const PxU32 nbTris = data.mNbVerts - 2;
					const PxU8 vref0 = polygons[data.mIndexBase + 0];
					PX_ASSERT(vref0 < nbVerts);
					for (PxU32 j = 0; j < nbTris; j++)
					{
						const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
						const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

						result.push_back(vec3(vertices[vref0].x, vertices[vref0].y, vertices[vref0].z) * vec3(scale.x, scale.y, scale.z));
						result.push_back(vec3(vertices[vref1].x, vertices[vref1].y, vertices[vref1].z) * vec3(scale.x, scale.y, scale.z));
						result.push_back(vec3(vertices[vref2].x, vertices[vref2].y, vertices[vref2].z) * vec3(scale.x, scale.y, scale.z));
					}
				}
			} break;
			}
		}

		return result;
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

	vector<vector<vec3>> Vehicle::GetWheelVertices()
	{
		vector<vector<vec3>> result;

		for (PxShape* wheel : this->wheels)
		{
			PxTransform pose = wheel->getLocalPose();

			PxGeometryHolder geometry = wheel->getGeometry();

			vector<vec3> wheelVertices;

			switch (geometry.getType())
			{
			case PxGeometryType::eCONVEXMESH:
			{
				//Compute triangles for each polygon.
				const PxVec3 scale = geometry.convexMesh().scale.scale;
				PxConvexMesh* mesh = geometry.convexMesh().convexMesh;
				const PxU32 nbPolys = mesh->getNbPolygons();
				const PxU8* polygons = mesh->getIndexBuffer();
				const PxVec3* verts = mesh->getVertices();
				PxU32 nbVerts = mesh->getNbVertices();
				PX_UNUSED(nbVerts);

				PxU32 numTotalTriangles = 0;
				for (PxU32 i = 0; i < nbPolys; i++)
				{
					PxHullPolygon data;
					mesh->getPolygonData(i, data);

					const PxU32 nbTris = data.mNbVerts - 2;
					const PxU8 vref0 = polygons[data.mIndexBase + 0];
					PX_ASSERT(vref0 < nbVerts);
					for (PxU32 j = 0; j < nbTris; j++)
					{
						const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
						const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

						wheelVertices.push_back(vec3(verts[vref0].x, verts[vref0].y, verts[vref0].z) * vec3(scale.x, scale.y, scale.z));
						wheelVertices.push_back(vec3(verts[vref1].x, verts[vref1].y, verts[vref1].z) * vec3(scale.x, scale.y, scale.z));
						wheelVertices.push_back(vec3(verts[vref2].x, verts[vref2].y, verts[vref2].z) * vec3(scale.x, scale.y, scale.z));
					}
				}
			} break;
			}
			result.push_back(wheelVertices);
		}

		return result;
	}
	vector<vec3> Vehicle::GetWheelPositions()
	{
		vector<vec3> result;

		for (PxShape* wheel : this->wheels)
		{
			PxVec3 position = wheel->getLocalPose().p;
			result.push_back(vec3(position.x, position.y, position.z));
		}

		return result;
	}
	vector<quat> Vehicle::GetWheelRotations()
	{
		vector<quat> result;

		for (PxShape* wheel : this->wheels)
		{
			PxQuat rotation = wheel->getLocalPose().q;
			result.push_back(quat(rotation.w, rotation.x, rotation.y, rotation.z));
		}

		return result;
	}
	vector<vec3> Vehicle::GetChassisVertices()
	{
		vector<vec3> result;

		PxTransform pose = chassis[0]->getLocalPose();

		PxGeometryHolder geometry = chassis[0]->getGeometry();

		switch (geometry.getType())
		{
		case PxGeometryType::eCONVEXMESH:
		{
			//Compute triangles for each polygon.
			const PxVec3 scale = geometry.convexMesh().scale.scale;
			PxConvexMesh* mesh = geometry.convexMesh().convexMesh;
			const PxU32 nbPolys = mesh->getNbPolygons();
			const PxU8* polygons = mesh->getIndexBuffer();
			const PxVec3* verts = mesh->getVertices();
			PxU32 nbVerts = mesh->getNbVertices();
			PX_UNUSED(nbVerts);

			PxU32 numTotalTriangles = 0;
			for (PxU32 i = 0; i < nbPolys; i++)
			{
				PxHullPolygon data;
				mesh->getPolygonData(i, data);

				const PxU32 nbTris = data.mNbVerts - 2;
				const PxU8 vref0 = polygons[data.mIndexBase + 0];
				PX_ASSERT(vref0 < nbVerts);
				for (PxU32 j = 0; j < nbTris; j++)
				{
					const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
					const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

					result.push_back(vec3(verts[vref0].x, verts[vref0].y, verts[vref0].z) * vec3(scale.x, scale.y, scale.z));
					result.push_back(vec3(verts[vref1].x, verts[vref1].y, verts[vref1].z) * vec3(scale.x, scale.y, scale.z));
					result.push_back(vec3(verts[vref2].x, verts[vref2].y, verts[vref2].z) * vec3(scale.x, scale.y, scale.z));
				}
			}
		} break;
		}
		return result;
	}
	vec3 Vehicle::GetChassisPosition()
	{
		PxVec3 position = this->chassis[0]->getLocalPose().p;
		return vec3(position.x, position.y, position.z);
	}
	quat Vehicle::GetChassisRotation()
	{
		PxQuat rotation = this->chassis[0]->getLocalPose().q;
		return quat(rotation.w, rotation.x, rotation.y, rotation.z);
	}
}