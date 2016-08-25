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
		void LoadData(int width, int height, int internalFormat, int format, int type, void* data);

		///<summary>Attaches the texture to the currently bound frame buffer object.</summary>
		void AttachToFramebuffer();
	private:
		GLuint id;
	};
}