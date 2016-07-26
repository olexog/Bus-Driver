#include "PositionedGeometry.h"

namespace PhysicsLibrary
{
	PositionedGeometry::PositionedGeometry(PxGeometry* geometry, PxVec3 position)
	{
		this->geometry = geometry;
		this->position = position;
	}

	PositionedGeometry::~PositionedGeometry()
	{

	}

	PxGeometry* PositionedGeometry::GetGeometry()
	{
		return this->geometry;
	}

	PxVec3 PositionedGeometry::GetPosition()
	{
		return this->position;
	}
}