#include "ColouredVertexArray.h"

namespace GraphicsLibrary
{
	ColouredVertexArray::ColouredVertexArray(vector<vec3> vertices, vector<vec3> colors)
		: VertexArray(vertices)
	{
		glBindVertexArray(this->id);
		this->colors = new Buffer();
		this->colors->Bind();
		this->colors->LoadData(colors);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(1);
		Buffer::Unbind();
		glBindVertexArray(0);

		this->vertexCount = vertices.size();
	}

	ColouredVertexArray::~ColouredVertexArray()
	{

	}

	void ColouredVertexArray::Draw(ShaderProgram* shaderProgram)
	{
		shaderProgram->Use();

		glBindVertexArray(this->id);
		glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
		glBindVertexArray(0);

		ShaderProgram::Disable();
	}
}