#include "Model.h"

namespace GraphicsLibrary
{
	Model::Model(vector<Collection*> collections)
	{
		this->collections = collections;
	}

	Model::~Model()
	{
		for (Collection* collection : this->collections)
		{
			delete collection;
		}
	}

	void Model::Draw(ShaderProgram* shaderProgram)
	{
		for (Collection* collection : this->collections)
		{
			collection->Draw(shaderProgram);
		}
	}
}
