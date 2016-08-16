#include "DynamicActor.h"

namespace PhysicsLibrary
{
	DynamicActor::DynamicActor(Physics* physics, Body* body, vec3* position, quat* orientation)
	{
		this->body = body;
		this->position = position;
		this->orientation = orientation;

		PxTransform globalPose = PxTransform(PxVec3(position->x, position->y, position->z), PxQuat(orientation->x, orientation->y, orientation->z, orientation->w));
		this->actor = physics->GetPhysics()->createRigidDynamic(globalPose);
		this->body->AddToActor(physics, this->actor);
	}

	DynamicActor::~DynamicActor()
	{
		this->actor->release();
	}

	void DynamicActor::Update()
	{
		this->position->x = this->actor->getGlobalPose().p.x;
		this->position->y = this->actor->getGlobalPose().p.y;
		this->position->z = this->actor->getGlobalPose().p.z;

		this->orientation->x = this->actor->getGlobalPose().q.x;
		this->orientation->y = this->actor->getGlobalPose().q.y;
		this->orientation->z = this->actor->getGlobalPose().q.z;
		this->orientation->w = this->actor->getGlobalPose().q.w;
	}

	void DynamicActor::AddToScene(PxScene* scene)
	{
		scene->addActor(*this->actor);
	}
}