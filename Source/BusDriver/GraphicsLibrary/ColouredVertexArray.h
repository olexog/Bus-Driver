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
		ColouredVertexArray(vector<vec3> vertices, vector<vec3> normals, vector<vec3> ambientColours, vector<vec3> diffuseColours, vector<vec3> specularColours);
		~ColouredVertexArray();
		void Draw(ShaderProgram* shaderProgram);
	private:
		Buffer* ambientColours;
		Buffer* diffuseColours;
		Buffer* specularColours;
		int vertexCount;
	};
}