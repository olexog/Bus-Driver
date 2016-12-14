#include "Segment.h"

namespace GraphicsLibrary
{
	Segment::Segment(vec3 colour)
	{
		this->colour = colour;

		glGenVertexArrays(1, &this->vertexArrayId);
		glBindVertexArray(this->vertexArrayId);

		this->vertexBuffer = new VertexBuffer();
		this->vertexBuffer->Bind();
		this->vertexBuffer->LoadData({ vec3(1.0f), vec3(0.0f) });
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(vec3), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(0);
		VertexBuffer::Unbind();

		glBindVertexArray(0);
	}

	Segment::~Segment()
	{

	}

	void Segment::Draw(ShaderProgram* shaderProgram)
	{
		shaderProgram->SetUniform("colour", this->colour);
		shaderProgram->Use();

		glBindVertexArray(this->vertexArrayId);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}
}