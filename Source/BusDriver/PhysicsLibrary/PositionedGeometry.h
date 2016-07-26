#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

namespace PhysicsLibrary
{
	class PositionedGeometry
	{
	public:
		PositionedGeometry(PxGeometry* geometry, PxVec3 position);
		~PositionedGeometry();

		PxGeometry* GetGeometry();

		PxVec3 GetPosition();
	private:
		PxGeometry* geometry;
		PxVec3 position;
	};
}