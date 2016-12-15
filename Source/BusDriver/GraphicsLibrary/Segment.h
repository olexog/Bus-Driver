#pragma once

#include "ShaderProgram.h"
#include "VertexArray.h"

namespace GraphicsLibrary
{
	class Segment
	{
	public:
		Segment();
		~Segment();

		void Draw(ShaderProgram* shaderProgram, vec3 startPoint, vec3 endPoint, vec3 colour);
	private:
		GLuint vertexArrayId;
		VertexBuffer* vertexBuffer;
	};
}