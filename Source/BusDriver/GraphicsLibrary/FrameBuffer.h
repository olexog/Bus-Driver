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

		bool IsComplete();

		static void Unbind();
	private:
		GLuint id;
	};
}