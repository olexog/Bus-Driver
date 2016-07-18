#pragma once

#include <vector>

#include "Collection.h"
#include "ShaderProgram.h"

using namespace std;

namespace GraphicsLibrary
{
	class Model
	{
	public:
		Model(vector<Collection*> collections);
		~Model();

		void Draw(ShaderProgram* shaderProgram);
	private:
		vector<Collection*> collections;
	};
}

