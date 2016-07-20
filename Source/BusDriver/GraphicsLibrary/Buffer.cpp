#include "Buffer.h"

namespace GraphicsLibrary
{
	Buffer::Buffer(vector<PxVec2> elements)
	{
		glGenBuffers(1, &this->id);

		glBindBuffer(GL_ARRAY_BUFFER, this->id);
		{
			glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(PxVec2), elements.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(PxVec2), (GLvoid*)0);
			glEnableVertexAttribArray(0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Buffer::Buffer(vector<PxVec3> elements)
	{
		glGenBuffers(1, &this->id);

		glBindBuffer(GL_ARRAY_BUFFER, this->id);
		{
			glBufferData(GL_ARRAY_BUFFER, elements.size() * sizeof(PxVec3), elements.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PxVec3), (GLvoid*)0);
			glEnableVertexAttribArray(0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Buffer::~Buffer()
	{
		glDeleteBuffers(1, &this->id);
	}
}
