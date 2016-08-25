#include "Model.h"


namespace GraphicsLibrary
{
	Model::Model(vector<VertexArray*> vertexArrays)
	{
		this->vertexArrays = vertexArrays;
	}

	Model::~Model()
	{
		for (VertexArray* vertexArray : this->vertexArrays)
		{
			delete vertexArray;
		}
	}

	void Model::Draw(ShaderProgram* shaderProgram)
	{
		for (VertexArray* vertexArray : this->vertexArrays)
		{
			vertexArray->Draw(shaderProgram);
		}
	}
}
