#pragma once

#include "VertexArray.h"
#include "ShaderProgram.h"

namespace GraphicsLibrary
{
	class Model
	{
	public:
		Model(vector<VertexArray*> vertexArrays);
		~Model();

		void Draw(ShaderProgram* shaderProgram);
	// TO DEBUG
	//private:
		vector<VertexArray*> vertexArrays;
	};
}

