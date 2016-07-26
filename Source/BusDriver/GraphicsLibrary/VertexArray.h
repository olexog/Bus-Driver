#pragma once

#include <vector>

#include <GL/glew.h>

#include <glm\vec3.hpp>

#include "VertexBuffer.h"
#include "ShaderProgram.h"

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	class VertexArray
	{
	public:
		VertexArray(vector<vec3> vertices, vector<vec3> normals);
		~VertexArray();
		virtual void Draw(ShaderProgram* shaderProgram);
		// TO DEBUG
		vector<vec3> debugVertices;
	protected:
		GLuint id;
		int vertexCount;
	private:
		VertexBuffer* vertices;
		VertexBuffer* normals;
	};
}