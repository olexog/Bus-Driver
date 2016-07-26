#pragma once

#include <GL/glew.h>

namespace GraphicsLibrary
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		void Bind();
		
		static void Unbind();

		void LoadData(int width, int height, void* data);

		void AttachToFramebuffer();
	private:
		GLuint id;
	};
}