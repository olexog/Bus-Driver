#include "Actor.h"

PhysicsLibrary::Actor::Actor(Physics* physics, Body* body, vec3* position, quat* orientation)
{
	this->body = body;
	this->position = position;
	this->orientation = orientation;

	PxVec3 pxPosition = PxVec3(position->x, position->y, position->z);
	PxQuat pxOrientation = PxQuat(orientation->x, orientation->y, orientation->z, orientation->w);

	this->actor = physics->GetPhysics()->createRigidStatic(PxTransform(pxPosition, pxOrientation));
	this->body->AddToActor(physics, this->actor);
}

void PhysicsLibrary::Actor::AddToScene(PxScene* scene)
{
	scene->addActor(*this->actor);
}

PhysicsLibrary::Actor::~Actor()
{
	this->actor->release();
}
