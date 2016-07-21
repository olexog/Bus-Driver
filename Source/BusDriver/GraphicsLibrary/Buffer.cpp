#include "Buffer.h"

namespace GraphicsLibrary
{
	Buffer::Buffer()
	{
		glGenBuffers(1, &this->id);
	}

	Buffer::~Buffer()
	{
		glDeleteBuffers(1, &this->id);
	}

	void Buffer::LoadData(vector<vec2> elements)
	{
		glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(vec2), elements.data(), GL_STATIC_DRAW);
	}

	void Buffer::LoadData(vector<vec3> elements)
	{
		glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(vec3), elements.data(), GL_STATIC_DRAW);
	}

	void Buffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->id);
	}

	void Buffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
