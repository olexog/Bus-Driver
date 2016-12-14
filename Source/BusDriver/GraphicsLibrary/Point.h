#pragma once

#include "ShaderProgram.h"
#include "VertexArray.h"

namespace GraphicsLibrary
{
	class Point
	{
	public:
		Point(float size, vec3 colour);
		~Point();

		void Draw(ShaderProgram* shaderProgram);
	private:
		GLuint vertexArrayId;
		VertexBuffer* vertexBuffer;
		float size;
		vec3 colour;
	};
}