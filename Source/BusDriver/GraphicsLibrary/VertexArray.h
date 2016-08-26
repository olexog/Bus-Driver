#pragma once

#include <vector>

#include <GL/glew.h>

#include <glm\vec2.hpp>
#include <glm\vec3.hpp>

#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "Texture.h"

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	class VertexArray
	{
	public:
		VertexArray(vector<vec3> vertices, vector<vec3> normals, vector<vec2> texCoords, Texture* texture);
		~VertexArray();
		void Draw(ShaderProgram* shaderProgram);
	private:
		GLuint id;
		int vertexCount;
		VertexBuffer* vertices;
		VertexBuffer* normals;
		VertexBuffer* texCoords;
		Texture* texture;
	};
}