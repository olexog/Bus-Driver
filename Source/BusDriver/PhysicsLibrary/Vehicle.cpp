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

		//Create the friction table for each combination of tire and surface type.
		gFrictionPairs = createFrictionPairs(physics->GetMaterial());

		//Create a plane to drive on.
		gGroundPlane = createDrivablePlane(physics->GetMaterial(), physics->GetPhysics());

		VehicleDesc vehicleDesc = initVehicleDesc(physics->GetMaterial());

		//Construct a physx actor with shapes for the chassis and wheels.
		//Set the rigid body mass, moment of inertia, and center of mass offset.
		PxRigidDynamic* veh4WActor = NULL;
		{
			//Construct a convex mesh for a cylindrical wheel.
			PxConvexMesh* wheelMesh = createWheelMesh(vehicleDesc.wheelWidth, vehicleDesc.wheelRadius, *physics->GetPhysics(), *physics->GetCooking());

			//Assume all wheels are identical for simplicity.
			PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
			PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

			//Set the meshes and materials for the driven wheels.
			for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelConvexMeshes[i] = wheelMesh;
				wheelMaterials[i] = vehicleDesc.wheelMaterial;
			}
			//Set the meshes and materials for the non-driven wheels
			for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < vehicleDesc.numWheels; i++)
			{
				wheelConvexMeshes[i] = wheelMesh;
				wheelMaterials[i] = vehicleDesc.wheelMaterial;
			}

			//Chassis just has a single convex shape for simplicity.
			PxConvexMesh* chassisConvexMesh = createChassisMesh(vehicleDesc.chassisDims, *physics->GetPhysics(), *physics->GetCooking());
			PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
			PxMaterial* chassisMaterials[1] = { vehicleDesc.chassisMaterial };

			//Rigid body data.
			PxVehicleChassisData rigidBodyData;
			rigidBodyData.mMOI = vehicleDesc.chassisMOI;
			rigidBodyData.mMass = vehicleDesc.chassisMass;
			rigidBodyData.mCMOffset = vehicleDesc.chassisCMOffset;

			//We need a rigid body actor for the vehicle.
			//Don't forget to add the actor to the scene after setting up the associated vehicle.
			veh4WActor = physics->GetPhysics()->createRigidDynamic(PxTransform(PxIdentity));

			//Wheel and chassis simulation filter data.
			PxFilterData wheelSimFilterData;
			wheelSimFilterData.word0 = COLLISION_FLAG_WHEEL;
			wheelSimFilterData.word1 = COLLISION_FLAG_WHEEL_AGAINST;
			PxFilterData chassisSimFilterData;
			chassisSimFilterData.word0 = COLLISION_FLAG_CHASSIS;
			chassisSimFilterData.word1 = COLLISION_FLAG_CHASSIS_AGAINST;

			//Wheel and chassis query filter data.
			//Optional: cars don't drive on other cars.
			PxFilterData wheelQryFilterData;
			setupNonDrivableSurface(wheelQryFilterData);
			PxFilterData chassisQryFilterData;
			setupNonDrivableSurface(chassisQryFilterData);

			//Add all the wheel shapes to the actor.
			for (PxU32 i = 0; i < vehicleDesc.numWheels; i++)
			{
				PxConvexMeshGeometry* geom = new PxConvexMeshGeometry(wheelConvexMeshes[i]);
				Shape* wheel = new Shape(physics, geom, PxVec3(), PxQuat());
				PxShape* wheelShape = veh4WActor->createShape(*geom, *wheelMaterials[i]);
				wheelShape->setQueryFilterData(wheelQryFilterData);
				wheelShape->setSimulationFilterData(wheelSimFilterData);
				wheelShape->setLocalPose(PxTransform(PxIdentity));

				wheels.push_back(wheelShape);
			}

			//Add the chassis shapes to the actor.
			for (PxU32 i = 0; i < 1; i++)
			{
				PxShape* chassisShape = veh4WActor->createShape
					(PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);
				chassisShape->setQueryFilterData(chassisQryFilterData);
				chassisShape->setSimulationFilterData(chassisSimFilterData);
				chassisShape->setLocalPose(PxTransform(PxVec3(0.0f, -0.5f, 0.0f)));

				chassis.push_back(chassisShape);
			}

			veh4WActor->setMass(vehicleDesc.chassisMass);
			veh4WActor->setMassSpaceInertiaTensor(vehicleDesc.chassisMOI);
			veh4WActor->setCMassLocalPose(PxTransform(vehicleDesc.chassisCMOffset, PxQuat(PxIdentity)));
		}

		//Set up the sim data for the wheels.
		PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(vehicleDesc.numWheels);
		{
			//Compute the wheel center offsets from the origin.
			PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
			const PxF32 frontZ = vehicleDesc.chassisDims.z*0.3f;
			const PxF32 rearZ = -vehicleDesc.chassisDims.z*0.3f;

			//chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
			//The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
			//Compute a position for the front wheel and the rear wheel along the z-axis.
			//Compute the separation between each wheel along the z-axis.
			const PxF32 numLeftWheels = vehicleDesc.numWheels / 2.0f;
			const PxF32 deltaZ = (frontZ - rearZ) / (numLeftWheels - 1.0f);
			//Set the outside of the left and right wheels to be flush with the chassis.
			//Set the top of the wheel to be just touching the underside of the chassis.
			//Begin by setting the rear-left/rear-right/front-left,front-right wheels.
			wheelCenterActorOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT] = PxVec3((-vehicleDesc.chassisDims.x + vehicleDesc.wheelWidth)*0.5f, -(vehicleDesc.chassisDims.y / 2 + vehicleDesc.wheelRadius), rearZ + 0 * deltaZ*0.5f);
			wheelCenterActorOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = PxVec3((+vehicleDesc.chassisDims.x - vehicleDesc.wheelWidth)*0.5f, -(vehicleDesc.chassisDims.y / 2 + vehicleDesc.wheelRadius), rearZ + 0 * deltaZ*0.5f);
			wheelCenterActorOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = PxVec3((-vehicleDesc.chassisDims.x + vehicleDesc.wheelWidth)*0.5f, -(vehicleDesc.chassisDims.y / 2 + vehicleDesc.wheelRadius), rearZ + (numLeftWheels - 1)*deltaZ);
			wheelCenterActorOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = PxVec3((+vehicleDesc.chassisDims.x - vehicleDesc.wheelWidth)*0.5f, -(vehicleDesc.chassisDims.y / 2 + vehicleDesc.wheelRadius), rearZ + (numLeftWheels - 1)*deltaZ);
			//Set the remaining wheels.
			for (PxU32 i = 2, wheelCount = 4; i < vehicleDesc.numWheels - 2; i += 2, wheelCount += 2)
			{
				wheelCenterActorOffsets[wheelCount + 0] = PxVec3((-vehicleDesc.chassisDims.x + vehicleDesc.wheelWidth)*0.5f, -(vehicleDesc.chassisDims.y / 2 + vehicleDesc.wheelRadius), rearZ + i*deltaZ*0.5f);
				wheelCenterActorOffsets[wheelCount + 1] = PxVec3((+vehicleDesc.chassisDims.x - vehicleDesc.wheelWidth)*0.5f, -(vehicleDesc.chassisDims.y / 2 + vehicleDesc.wheelRadius), rearZ + i*deltaZ*0.5f);
			}

			//Set up the wheels.
			PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
			{
				//Set up the wheel data structures with mass, moi, radius, width.
				for (PxU32 i = 0; i < vehicleDesc.numWheels; i++)
				{
					wheels[i].mMass = vehicleDesc.wheelMass;
					wheels[i].mMOI = vehicleDesc.wheelMOI;
					wheels[i].mRadius = vehicleDesc.wheelRadius;
					wheels[i].mWidth = vehicleDesc.wheelWidth;
				}

				//Enable the handbrake for the rear wheels only.
				wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
				wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
				//Enable steering for the front wheels only.
				wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi*0.3333f;
				wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi*0.3333f;
			}

			//Set up the tires.
			PxVehicleTireData tires[PX_MAX_NB_WHEELS];
			{
				//Set up the tires.
				for (PxU32 i = 0; i < vehicleDesc.numWheels; i++)
				{
					tires[i].mType = TIRE_TYPE_NORMAL;
				}
			}

			//Set up the suspensions
			PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
			{
				//Compute the mass supported by each suspension spring.
				PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
				PxVehicleComputeSprungMasses
					(vehicleDesc.numWheels, wheelCenterActorOffsets,
						vehicleDesc.chassisCMOffset, vehicleDesc.chassisMass, 1, suspSprungMasses);

				//Set the suspension data.
				for (PxU32 i = 0; i < vehicleDesc.numWheels; i++)
				{
					suspensions[i].mMaxCompression = 0.3f;
					suspensions[i].mMaxDroop = 0.1f;
					suspensions[i].mSpringStrength = 35000.0f;
					suspensions[i].mSpringDamperRate = 4500.0f;
					suspensions[i].mSprungMass = suspSprungMasses[i];
				}

				//Set the camber angles.
				const PxF32 camberAngleAtRest = 0.0;
				const PxF32 camberAngleAtMaxDroop = 0.01f;
				const PxF32 camberAngleAtMaxCompression = -0.01f;
				for (PxU32 i = 0; i < vehicleDesc.numWheels; i += 2)
				{
					suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
					suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
					suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
					suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
					suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
					suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
				}
			}

			//Set up the wheel geometry.
			PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
			PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
			PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
			PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
			{
				//Set the geometry data.
				for (PxU32 i = 0; i < vehicleDesc.numWheels; i++)
				{
					//Vertical suspension travel.
					suspTravelDirections[i] = PxVec3(0, -1, 0);

					//Wheel center offset is offset from rigid body center of mass.
					wheelCentreCMOffsets[i] =
						wheelCenterActorOffsets[i] - vehicleDesc.chassisCMOffset;

					//Suspension force application point 0.3 metres below 
					//rigid body center of mass.
					suspForceAppCMOffsets[i] =
						PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

					//Tire force application point 0.3 metres below 
					//rigid body center of mass.
					tireForceAppCMOffsets[i] =
						PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
				}
			}

			//Set up the filter data of the raycast that will be issued by each suspension.
			PxFilterData qryFilterData;
			setupNonDrivableSurface(qryFilterData);

			//Set the wheel, tire and suspension data.
			//Set the geometry data.
			//Set the query filter data
			for (PxU32 i = 0; i < vehicleDesc.numWheels; i++)
			{
				wheelsSimData->setWheelData(i, wheels[i]);
				wheelsSimData->setTireData(i, tires[i]);
				wheelsSimData->setSuspensionData(i, suspensions[i]);
				wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
				wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
				wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
				wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
				wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
				wheelsSimData->setWheelShapeMapping(i, i);
			}
		}

		//Set up the sim data for the vehicle drive model.
		PxVehicleDriveSimData4W driveSimData;
		{
			//Diff
			PxVehicleDifferential4WData diff;
			diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
			driveSimData.setDiffData(diff);

			//Engine
			PxVehicleEngineData engine;
			engine.mPeakTorque = 500.0f;
			engine.mMaxOmega = 600.0f;//approx 6000 rpm
			driveSimData.setEngineData(engine);

			//Gears
			PxVehicleGearsData gears;
			gears.mSwitchTime = 0.5f;
			driveSimData.setGearsData(gears);

			//Clutch
			PxVehicleClutchData clutch;
			clutch.mStrength = 10.0f;
			driveSimData.setClutchData(clutch);

			//Ackermann steer accuracy
			PxVehicleAckermannGeometryData ackermann;
			ackermann.mAccuracy = 1.0f;
			ackermann.mAxleSeparation =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
			ackermann.mFrontWidth =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
			ackermann.mRearWidth =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
			driveSimData.setAckermannGeometryData(ackermann);
		}

		//Create a vehicle from the wheels and drive sim data.
		gVehicle4W = PxVehicleDrive4W::allocate(vehicleDesc.numWheels);
		gVehicle4W->setup(physics->GetPhysics(), veh4WActor, *wheelsSimData, driveSimData, vehicleDesc.numWheels - 4);

		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

		PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxPi, PxVec3(0, 1, 0)));
		gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	}

	Vehicle::~Vehicle()
	{
	}

	void Vehicle::SetToRestState()
	{
		gVehicle4W->setToRestState();
		gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		gVehicle4W->mDriveDynData.setUseAutoGears(true);
	}

	void Vehicle::Update(float elapsedTime, PxBatchQuery* batchQuery, PxRaycastQueryResult* raycastResults, PxU32 raycastResultsSize, PxVec3 gravity)
	{
		// suspension raycasts
		PxVehicleWheels* vehicles[1] = { gVehicle4W };
		PxVehicleSuspensionRaycasts(batchQuery, 1, vehicles, raycastResultsSize, raycastResults);

		// update vehicle
		PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		PxVehicleWheelQueryResult vehicleQueryResults[1] = { { wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels() } };
		PxVehicleUpdates(elapsedTime, gravity, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

		//Work out if the vehicle is in the air.
		gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
	}

	void Vehicle::UpdateInput(float elapsedTime)
	{
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, elapsedTime, gIsVehicleInAir, *gVehicle4W);
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

	void Vehicle::Handbrake(float measure)
	{
		this->gVehicleInputData.setAnalogHandbrake(measure);
	}

	void Vehicle::AddToScene(PxScene* scene)
	{
		scene->addActor(*gVehicle4W->getRigidDynamicActor());
		scene->addActor(*gGroundPlane);
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