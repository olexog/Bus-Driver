#include "ColouredVertexArray.h"

namespace GraphicsLibrary
{
	ColouredVertexArray::ColouredVertexArray(vector<PxVec3> vertices, vector<PxVec3> colors)
		: VertexArray(vertices)
	{
		glBindVertexArray(this->id);
		this->colors = new Buffer();
		this->colors->Bind();
		this->colors->LoadData(colors);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PxVec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(1);
		Buffer::Unbind();
		glBindVertexArray(0);
	}

	ColouredVertexArray::~ColouredVertexArray()
	{

	}

	void ColouredVertexArray::Draw(ShaderProgram* shaderProgram)
	{
		shaderProgram->Use();

		glBindVertexArray(this->id);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		ShaderProgram::Disable();
	}
}