#include "Actor.h"

namespace PhysicsLibrary
{
	Actor::Actor(Physics* physics, vec3 halfExtends, vec3 position)
	{
		PxVec3 Pxposition = PxVec3(position.x, position.y, position.z);
		this->actor = physics->GetPhysics()->createRigidDynamic(PxTransform(Pxposition));
		PxShape* cubeShape = this->actor->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *physics->GetMaterial(), PxTransform(PxIdentity));
		cubeShape->setSimulationFilterData(physics->obstacleSimFilterData);
		physics->AddActor(*this->actor);
	}

	Actor::~Actor()
	{
	}
	
	vec3 Actor::GetPosition()
	{
		PxVec3 position = this->actor->getGlobalPose().p;
		return vec3(position.x, position.y, position.z);
	}

	quat Actor::GetRotation()
	{
		PxQuat rotation = this->actor->getGlobalPose().q;
		return quat(rotation.w, rotation.x, rotation.y, rotation.z);
	}
}