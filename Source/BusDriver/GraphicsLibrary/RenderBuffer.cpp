#include "RenderBuffer.h"

namespace GraphicsLibrary
{
	RenderBuffer::RenderBuffer()
	{
		glGenRenderbuffers(1, &this->id);
	}

	RenderBuffer::~RenderBuffer()
	{
		glDeleteRenderbuffers(1, &this->id);
	}

	void RenderBuffer::Initialize(PixelFormat internalFormat, int width, int height)
	{
		this->Bind();
		glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(internalFormat), width, height);
		RenderBuffer::Unbind();
	}

	void RenderBuffer::AttachToFrameBuffer(Attachment attachment)
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, static_cast<GLenum>(attachment), GL_RENDERBUFFER, this->id);
	}

	void RenderBuffer::Bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, this->id);
	}

	void RenderBuffer::Unbind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}