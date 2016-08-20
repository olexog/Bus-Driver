#include "Shape.h"

PhysicsLibrary::Shape::Shape(Physics* physics, PxGeometry* geometry)
{
	this->geometry = geometry;
	this->position = new vec3();
	this->orientation = new quat();
	this->type = Type::OBSTACLE;
}

PhysicsLibrary::Shape::Shape(Physics* physics, PxGeometry* geometry, vec3* position, quat* orientation, Type type)
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
	this->shape = actor->createShape(*this->geometry, *physics->GetMaterial());
	//shape = this->shape;

	PxTransform localPose = PxTransform(PxVec3(this->position->x, this->position->y, this->position->z), PxQuat(this->orientation->x, this->orientation->y, this->orientation->z, this->orientation->w));

	this->shape->setLocalPose(PxTransform(localPose));

	switch (type)
	{
	case CHASSIS:
	{
		this->shape->setSimulationFilterData(physics->GetChassisSimulationFilterData());
		this->shape->setQueryFilterData(physics->GetChassisQueryFilterData());
	} break;
	case WHEEL:
	{
		this->shape->setSimulationFilterData(physics->GetWheelSimulationFilterData());
		this->shape->setQueryFilterData(physics->GetWheelQueryFilterData());
	} break;
	case OBSTACLE:
	{
		this->shape->setSimulationFilterData(physics->GetDrivableObstacleSimulationFilterData());
		this->shape->setQueryFilterData(physics->GetDrivableObstacleQueryFilterData());
	} break;
	}
}

void PhysicsLibrary::Shape::Update()
{
	this->position->x = this->shape->getLocalPose().p.x;
	this->position->y = this->shape->getLocalPose().p.y;
	this->position->z = this->shape->getLocalPose().p.z;

	this->orientation->x = this->shape->getLocalPose().q.x;
	this->orientation->y = this->shape->getLocalPose().q.y;
	this->orientation->z = this->shape->getLocalPose().q.z;
	this->orientation->w = this->shape->getLocalPose().q.w;
}

void PhysicsLibrary::Shape::SetType(Type type)
{
	this->type = type;
}

void PhysicsLibrary::Shape::SetPosePointer(vec3* position, quat* orientation)
{
	this->position = position;
	this->orientation = orientation;
}
