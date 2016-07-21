#pragma once

#include <vector>

#include <glm\vec3.hpp>

#include "VertexArray.h"

using namespace std;

using namespace glm;

namespace GraphicsLibrary
{
	class ColouredVertexArray :
		public VertexArray
	{
	public:
		ColouredVertexArray(vector<vec3> vertices, vector<vec3> colors);
		~ColouredVertexArray();
		void Draw(ShaderProgram* shaderProgram);
	private:
		Buffer* colors;
	};
}