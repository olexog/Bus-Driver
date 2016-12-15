#pragma once

#include "ShaderProgram.h"
#include "VertexArray.h"

namespace GraphicsLibrary
{
	class Point
	{
	public:
		Point();
		~Point();

		void Draw(ShaderProgram* shaderProgram, vec3 position, float size, vec3 colour);
	private:
		GLuint vertexArrayId;
		VertexBuffer* vertexBuffer;
	};
}