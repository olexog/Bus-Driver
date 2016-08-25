#include "FrameBuffer.h"

namespace GraphicsLibrary
{
	FrameBuffer::FrameBuffer()
	{
		glGenFramebuffers(1, &this->id);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &this->id);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->id);
	}

	bool FrameBuffer::IsComplete()
	{
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}