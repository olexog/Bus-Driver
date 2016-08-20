#include "Shape.h"

PhysicsLibrary::Shape::Shape(Physics * physics, PxGeometry * geometry)
{
	this->geometry = geometry;
	this->position = PxVec3(PxIdentity);
	this->orientation = PxQuat(PxIdentity);
	this->type = Type::OBSTACLE;
}

PhysicsLibrary::Shape::Shape(Physics* physics, PxGeometry* geometry, PxVec3 position, PxQuat orientation, Type type)
{
	this->geometry = geometry;
	this->position = position;
	this->orientation = orientation;
	this->type = type;
}

PhysicsLibrary::Shape::~Shape()
{
	delete this->geometry;
}

void PhysicsLibrary::Shape::AddToActor(Physics* physics, PxRigidActor* actor, PxShape* &shape)
{
	shape = actor->createShape(*this->geometry, *physics->GetMaterial());
	shape->setLocalPose(PxTransform(this->position, this->orientation));

	switch (type)
	{
	case CHASSIS:
	{
		shape->setSimulationFilterData(physics->GetChassisSimulationFilterData());
		shape->setQueryFilterData(physics->GetChassisQueryFilterData());
	} break;
	case WHEEL:
	{
		shape->setSimulationFilterData(physics->GetWheelSimulationFilterData());
		shape->setQueryFilterData(physics->GetWheelQueryFilterData());
	} break;
	case OBSTACLE:
	{
		shape->setSimulationFilterData(physics->GetDrivableObstacleSimulationFilterData());
		shape->setQueryFilterData(physics->GetDrivableObstacleQueryFilterData());
	} break;
	}
}

void PhysicsLibrary::Shape::SetType(Type type)
{
	this->type = type;
}
