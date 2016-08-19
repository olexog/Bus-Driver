#include "Shape.h"

PhysicsLibrary::Shape::Shape(Physics * physics, PxGeometry * geometry)
{
	this->geometry = geometry;
	this->position = PxVec3(PxIdentity);
	this->orientation = PxQuat(PxIdentity);
}

PhysicsLibrary::Shape::Shape(Physics* physics, PxGeometry* geometry, PxVec3 position, PxQuat orientation)
{
	this->geometry = geometry;
	this->position = position;
	this->orientation = orientation;
}

PhysicsLibrary::Shape::~Shape()
{
	delete this->geometry;
}

void PhysicsLibrary::Shape::AddToActor(Physics* physics, PxRigidActor* actor)
{
	PxShape* shape = actor->createShape(*this->geometry, *physics->GetMaterial());
	shape->setLocalPose(PxTransform(this->position, this->orientation));
	shape->setSimulationFilterData(physics->GetDrivableObstacleSimulationFilterData());
	shape->setQueryFilterData(physics->GetDrivableObstacleQueryFilterData());
}
