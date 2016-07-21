#include "Model.h"


namespace GraphicsLibrary
{
	Model::Model(ColouredVertexArray* colouredVertexArray)
	{
		this->colouredVertexArray = colouredVertexArray;
	}

	Model::~Model()
	{
		delete this->colouredVertexArray;
	}

	void Model::Draw(ShaderProgram * shaderProgram, ShaderProgram* colouredShaderProgram)
	{
		this->colouredVertexArray->Draw(colouredShaderProgram);
	}
}
