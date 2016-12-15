#pragma once

#include <vector>

#include <GL\glew.h>

#include <glm\vec2.hpp>
#include <glm\vec3.hpp>

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	///<summary>The handling to an OpenGL Buffer Object.</summary>
	class VertexBuffer
	{
	public:
		///<summary>Initializes a new Buffer Object of two-dimensional vectors and attaches it to the bound Vertex Array Object.</summary>
		VertexBuffer();

		///<summary>Destroys the Buffer Object.</summary>
		~VertexBuffer();

		void LoadData(vector<vec2> elements);

		void LoadData(vector<vec3> elements);

		void PrepareDynamicData(int size);

		void LoadDataDynamic(vector<vec2> elements);

		void Bind();

		static void Unbind();
	private:
		GLuint id;
	};
}