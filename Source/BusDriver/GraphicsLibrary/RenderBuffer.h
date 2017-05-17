#pragma once

#include <gl\glew.h>

#include "PixelFormat.h"
#include "Attachment.h"

namespace GraphicsLibrary
{
	class RenderBuffer
	{
	public:
		RenderBuffer();
		~RenderBuffer();

		void Initialize(PixelFormat internalFormat, int width, int height);

		void AttachToFrameBuffer(Attachment attachment);
	private:
		GLuint id;

		void Bind();

		static void Unbind();
	};
}