#pragma once

#include <vector>

#include <GL\glew.h>

#include <PxPhysicsAPI.h>

using namespace std;
using namespace physx;

namespace GraphicsLibrary
{
	///<summary>The handling to an OpenGL Buffer Object.</summary>
	class Buffer
	{
	public:
		///<summary>Initializes a new Buffer Object of two-dimensional vectors and attaches it to the bound Vertex Array Object.</summary>
		Buffer(vector<PxVec2> elements);

		///<summary>Initializes a new Buffer Object of three-dimensional vectors and attaches it to the bound Vertex Array Object.</summary>
		Buffer(vector<PxVec3> elements);

		///<summary>Destroys the Buffer Object.</summary>
		~Buffer();
	private:
		unsigned int id;
	};
}