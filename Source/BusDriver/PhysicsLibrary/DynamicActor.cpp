#include "DynamicActor.h"

namespace PhysicsLibrary
{
	DynamicActor::DynamicActor(Physics* physics, vec3 halfExtends, vec3 position)
	{
		PxVec3 Pxposition = PxVec3(position.x, position.y, position.z);
		this->actor = physics->GetPhysics()->createRigidDynamic(PxTransform(Pxposition));
		PxShape* cubeShape = this->actor->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *physics->GetMaterial(), PxTransform(PxIdentity));
		cubeShape->setSimulationFilterData(physics->obstacleSimFilterData);
	}

	DynamicActor::~DynamicActor()
	{

	}

	void DynamicActor::AddToScene(PxScene* scene)
	{
		scene->addActor(*this->actor);
	}
	
	vec3 DynamicActor::GetPosition()
	{
		PxVec3 position = this->actor->getGlobalPose().p;
		return vec3(position.x, position.y, position.z);
	}

	quat DynamicActor::GetRotation()
	{
		PxQuat rotation = this->actor->getGlobalPose().q;
		return quat(rotation.w, rotation.x, rotation.y, rotation.z);
	}
}