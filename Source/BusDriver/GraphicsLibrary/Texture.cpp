#include "Texture.h"

namespace GraphicsLibrary
{
	Texture::Texture()
	{
		glGenTextures(1, &this->id);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &this->id);
	}

	void Texture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, this->id);
	}

	void Texture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::LoadData(int width, int height, void* data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	
	void Texture::LoadData(int width, int height, int internalFormat, int format, int type, void* data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
	}

	void Texture::AttachToFramebuffer(int attachment)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, this->id, 0);
	}
}