#include "VertexBuffer.h"

namespace GraphicsLibrary
{
	VertexBuffer::VertexBuffer()
	{
		glGenBuffers(1, &this->id);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &this->id);
	}

	void VertexBuffer::LoadData(vector<vec2> elements)
	{
		glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(vec2), elements.data(), GL_STATIC_DRAW);
	}

	void VertexBuffer::LoadData(vector<vec3> elements)
	{
		glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(vec3), elements.data(), GL_STATIC_DRAW);
	}

	void VertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->id);
	}

	void VertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
