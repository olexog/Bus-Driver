#pragma once

#include <vector>

#include <PxPhysics.h>

#include "VertexArray.h"

using namespace std;

using namespace physx;

namespace GraphicsLibrary
{
	class ColouredVertexArray :
		public VertexArray
	{
	public:
		ColouredVertexArray(vector<PxVec3> vertices, vector<PxVec3> colors);
		~ColouredVertexArray();
		void Draw(ShaderProgram* shaderProgram);
	private:
		Buffer* colors;
	};
}