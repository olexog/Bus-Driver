#include "Point.h"

namespace GraphicsLibrary
{
	Point::Point()
	{
		glEnable(GL_PROGRAM_POINT_SIZE);

		glGenVertexArrays(1, &this->vertexArrayId);
		glBindVertexArray(this->vertexArrayId);

		this->vertexBuffer = new VertexBuffer();
		this->vertexBuffer->Bind();
		this->vertexBuffer->LoadData({ vec3(0.0f) });
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(0);
		VertexBuffer::Unbind();

		glBindVertexArray(0);
	}

	Point::~Point()
	{

	}

	void Point::Draw(ShaderProgram* shaderProgram, vec3 position, float size, vec3 colour)
	{
		shaderProgram->SetUniform("position", position);
		shaderProgram->SetUniform("pointSize", size);
		shaderProgram->SetUniform("colour", colour);
		shaderProgram->Use();

		glBindVertexArray(this->vertexArrayId);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	}
}