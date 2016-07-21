#pragma once

#include <vector>

#include <GL/glew.h>

#include <glm\vec3.hpp>

#include "Buffer.h"
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
	protected:
		GLuint id;
	private:
		Buffer* vertices;
		Buffer* normals;
	};
}