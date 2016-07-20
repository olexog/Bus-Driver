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
		Buffer();

		///<summary>Destroys the Buffer Object.</summary>
		~Buffer();

		void LoadData(vector<PxVec2> elements);

		void LoadData(vector<PxVec3> elements);

		void Bind();

		static void Unbind();
	private:
		GLuint id;
	};
}