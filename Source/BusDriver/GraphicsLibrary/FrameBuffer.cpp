#include "FrameBuffer.h"

namespace GraphicsLibrary
{
	FrameBuffer::FrameBuffer()
	{
		glGenBuffers(1, &this->id);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &this->id);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->id);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}