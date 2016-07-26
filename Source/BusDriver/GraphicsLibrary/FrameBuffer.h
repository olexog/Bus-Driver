#pragma once

#include <GL\glew.h>

namespace GraphicsLibrary
{
	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		void Bind();

		static void Unbind();
	private:
		GLuint id;
	};
}