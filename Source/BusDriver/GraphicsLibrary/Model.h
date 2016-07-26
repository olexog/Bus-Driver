#pragma once

#include "VertexArray.h"
#include "ColouredVertexArray.h"
#include "ShaderProgram.h"

namespace GraphicsLibrary
{
	class Model
	{
	public:
		Model(ColouredVertexArray* colouredVertexArray);
		~Model();

		void Draw(ShaderProgram* shaderProgram, ShaderProgram* colouredShaderProgram);
	// TO DEBUG
	//private:
		ColouredVertexArray* colouredVertexArray;
	};
}

