/*
* Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef SNIPPET_VEHICLE_COMMON_H
#define SNIPPET_VEHICLE_COMMON_H

#include <vector>

#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleNoDrive.h"

using namespace std;

using namespace physx;

////////////////////////////////////////////////

PxRigidStatic* createDrivablePlane(physx::PxMaterial* material, PxPhysics* physics);

PxTriangleMesh* createTriangleMesh(const vector<PxVec3> verts, const vector<PxU32> triangles, PxPhysics& physics, PxCooking& cooking);

////////////////////////////////////////////////

PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking);

PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking);

////////////////////////////////////////////////

PxRigidDynamic* createVehicleActor
(const PxVehicleChassisData& chassisData,
	PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, vector<PxShape*> &wheels, const PxU32 numWheels,
	PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, vector<PxShape*> &chassis, const PxU32 numChassisMeshes,
	PxPhysics& physics);

////////////////////////////////////////////////

struct VehicleDesc
{
	PxF32 chassisMass;
	PxVec3 chassisDims;
	PxVec3 chassisMOI;
	PxVec3 chassisCMOffset;
	PxMaterial* chassisMaterial;
	PxF32 wheelMass;
	PxF32 wheelWidth;
	PxF32 wheelRadius;
	PxF32 wheelMOI;
	PxMaterial* wheelMaterial;
	PxU32 numWheels;
};

PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehicle4WDesc, vector<PxShape*> &wheels, vector<PxShape*> &chassis, PxPhysics* physics, PxCooking* cooking);

////////////////////////////////////////////////

#endif //SNIPPET_VEHICLE_COMMON_H
