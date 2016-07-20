#include "VertexArray.h"

namespace GraphicsLibrary
{
	VertexArray::VertexArray(vector<PxVec3> vertices)
	{
		glGenVertexArrays(1, &this->id);

		glBindVertexArray(this->id);
		this->vertices = new Buffer();
		this->vertices->Bind();
		this->vertices->LoadData(vertices);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PxVec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(0);
		Buffer::Unbind();
		glBindVertexArray(0);
	}
	
	VertexArray::~VertexArray()
	{

	}

	void VertexArray::Draw(ShaderProgram* shaderProgram)
	{
		shaderProgram->Use();

		glBindVertexArray(this->id);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}
}