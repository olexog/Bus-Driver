#pragma once

#include <vector>

#include <GL/glew.h>

#include <PxPhysicsAPI.h>

#include "Buffer.h"
#include "ShaderProgram.h"

using namespace std;

using namespace physx;

namespace GraphicsLibrary
{
	class VertexArray
	{
	public:
		VertexArray(vector<PxVec3> vertices);
		~VertexArray();
		virtual void Draw(ShaderProgram* shaderProgram);
	protected:
		GLuint id;
	private:
		Buffer* vertices;
	};
}