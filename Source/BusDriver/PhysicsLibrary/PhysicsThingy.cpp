#include "PhysicsThingy.h"

namespace PhysicsLibrary
{
	PhysicsThingy::PhysicsThingy(Physics* physics, vec3 position, vector<PositionedGeometry> positionedGeometries)
	{
		PxVec3 Pxposition = PxVec3(position.x, position.y, position.z);
		this->actor = physics->GetPhysics()->createRigidDynamic(PxTransform(Pxposition));

		for (PositionedGeometry positionedGeometry : positionedGeometries)
		{
			PxGeometry* geometry = positionedGeometry.GetGeometry();
			PxTransform transform = PxTransform(positionedGeometry.GetPosition());
			PxShape* shape = this->actor->createShape(*geometry, *physics->GetMaterial(), transform);
			shape->setSimulationFilterData(physics->obstacleSimFilterData);
		}

		//physics->AddActor(*this->actor);
	}

	PhysicsThingy::~PhysicsThingy()
	{

	}
}