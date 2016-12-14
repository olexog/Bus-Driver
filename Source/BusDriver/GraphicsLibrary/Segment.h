#pragma once

#include "ShaderProgram.h"
#include "VertexArray.h"

namespace GraphicsLibrary
{
	class Segment
	{
	public:
		Segment(vec3 colour);
		~Segment();

		void Draw(ShaderProgram* shaderProgram);
	private:
		GLuint vertexArrayId;
		VertexBuffer* vertexBuffer;
		vec3 colour;
	};
}