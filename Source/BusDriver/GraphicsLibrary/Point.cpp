#include "Point.h"

namespace GraphicsLibrary
{
	Point::Point(float size, vec3 colour)
	{
		this->size = size;
		this->colour = colour;

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

	void Point::Draw(ShaderProgram* shaderProgram)
	{
		shaderProgram->SetUniform("pointSize", this->size);
		shaderProgram->SetUniform("colour", this->colour);
		shaderProgram->Use();

		glBindVertexArray(this->vertexArrayId);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	}
}