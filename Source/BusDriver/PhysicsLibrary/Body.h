#pragma once

#include <vector>

#include <glm\vec3.hpp>

#include <PxPhysicsAPI.h>

#include "Physics.h"
#include "Shape.h"

using namespace std;
using namespace glm;
using namespace physx;

namespace PhysicsLibrary
{
	class Body
	{
	public:
		Body(vector<Shape*> shapes);
		~Body();

		void AddToActor(Physics* physics, PxRigidActor* actor);
		void Update();
	private:
		vector<Shape*> shapes;
	};
}