#include "Collection.h"

namespace GraphicsLibrary
{
	Collection::Collection(vector<PxVec3> vertices)
	{
		glGenVertexArrays(1, &this->vertexArrayId);
		glGenBuffers(1, &this->vertexBufferId);

		glBindVertexArray(this->vertexArrayId);

		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PxVec3), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PxVec3), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

	}

	Collection::~Collection()
	{
		glDeleteVertexArrays(1, &this->vertexArrayId);
		glDeleteBuffers(1, &this->vertexBufferId);
	}

	void Collection::Draw(ShaderProgram* shaderProgram)
	{
		shaderProgram->Use();

		glBindVertexArray(this->vertexArrayId);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
}
