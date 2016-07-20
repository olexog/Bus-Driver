#include "Collection.h"

namespace GraphicsLibrary
{
	Collection::Collection(ShaderProgram* shaderProgram, vector<PxVec3> vertices)
	{
		glGenVertexArrays(1, &this->vertexArrayId);

		glBindVertexArray(this->vertexArrayId);
			this->vertexBuffer = new Buffer(vertices);
		glBindVertexArray(0);

		this->shaderProgram = shaderProgram;
	}

	Collection::~Collection()
	{
		glDeleteVertexArrays(1, &this->vertexArrayId);
		delete vertexBuffer;
	}

	void Collection::Draw()
	{
		this->shaderProgram->Use();

		glBindVertexArray(this->vertexArrayId);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
}
