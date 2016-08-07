#include "Body.h"

PhysicsLibrary::Body::Body(vector<Shape*> shapes)
{
	this->shapes = shapes;
}

PhysicsLibrary::Body::~Body()
{
	for (Shape* shape : this->shapes)
	{
		delete shape;
	}
}

void PhysicsLibrary::Body::AddToActor(Physics* physics, PxRigidActor* actor)
{
	for (Shape* shape : this->shapes)
	{
		shape->AddToActor(physics, actor);
	}
}
