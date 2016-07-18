#pragma once

#include <vector>

#include <GL\glew.h>

#include <PxPhysicsApi.h>

#include "ShaderProgram.h"

using namespace std;
using namespace physx;

namespace GraphicsLibrary
{
	class Collection
	{
	public:
		Collection(vector<PxVec3> vertices);
		~Collection();

		void Draw(ShaderProgram* shaderProgram);
	private:
		unsigned int vertexBufferId;
		unsigned int vertexArrayId;
	};
}
