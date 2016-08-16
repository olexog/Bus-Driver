#include "Material.h"

PhysicsLibrary::Material::Material(Physics* physics, float staticFriction, float dynamicFriction, float restitution)
{
	this->material = physics->GetPhysics()->createMaterial(staticFriction, dynamicFriction, restitution);
}

PhysicsLibrary::Material::~Material()
{
}
