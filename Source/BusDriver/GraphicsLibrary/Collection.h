#pragma once

#include <vector>

#include <GL\glew.h>

#include <PxPhysicsApi.h>

#include "ShaderProgram.h"
#include "Buffer.h"

using namespace std;
using namespace physx;

namespace GraphicsLibrary
{
	class Collection
	{
	public:
		Collection(ShaderProgram* shaderProgram, vector<PxVec3> vertices);
		~Collection();

		void Draw();
	private:
		Buffer* vertexBuffer;
		ShaderProgram* shaderProgram;
		unsigned int vertexArrayId;
	};
}