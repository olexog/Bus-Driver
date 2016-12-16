#pragma once

#include <iostream>
#include <string>
#include <map>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Texture.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	class Font
	{
	public:
		Font(string fontPath, int size, int firstCharacter, int lastCharacter);
		~Font();

		void DrawText(ShaderProgram* shaderProgram, string text, vec2 position, float scale, vec3 colour);
	private:
		struct Character
		{
		public:
			int startX;
			ivec2 size;
			ivec2 bearing;
			int advance;
			unsigned char* buffer;
			vector<vec2> texCoords;
		};
		map<int, Character*> characters;

		Texture* texture;
		VertexBuffer* vertices;
		VertexBuffer* textureCoordinates;
		GLuint vertexArrayId;
	};
}