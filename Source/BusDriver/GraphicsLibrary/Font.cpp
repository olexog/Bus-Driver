#include "Font.h"

namespace GraphicsLibrary
{
	Font::Font(string fontPath, int size, int firstCharacter, int lastCharacter)
	{
		// initialize FreeType library

		FT_Library library;
		FT_Error result = FT_Init_FreeType(&library);
		if (result)
		{
			cout << "FreeType library initialization has failed. Error code: " << result << endl;
			return;
		}

		// load face

		FT_Face face;
		result = FT_New_Face(library, fontPath.c_str(), 0, &face);
		if (result)
		{
			cout << "Face creation has failed. Error code: " << result << endl;
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, size);

		// setup necessary OpenGL stuff: alignment and blending

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// iterate and load each character

		int startX = 1;
		this->characters = map<int, Character*>();

		for (int c = firstCharacter; c <= lastCharacter; c++)
		{
			result = FT_Load_Char(face, c, FT_LOAD_RENDER);
			if (result)
			{
				cout << "Failed to load character with code " << c << ". Error code: " << result << endl;
			}

			Character* character = new Character();
			character->startX = startX;
			character->size = ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			character->bearing = ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top - face->glyph->bitmap.rows);
			character->advance = face->glyph->advance.x;

			startX += character->size.x + 1;

			int bufferSize = character->size.x * character->size.y * sizeof(unsigned char);
			unsigned char* buffer = new unsigned char[bufferSize];
			memcpy(buffer, face->glyph->bitmap.buffer, bufferSize);
			character->buffer = buffer;

			this->characters[c] = character;
		}

		// generate a big buffer containing all characters in the font

		int bigBufferWidth = 1;
		int bigBufferHeight = 0;
		for (pair<int, Character*> pair : this->characters)
		{
			bigBufferWidth += pair.second->size.x + 1;
			bigBufferHeight = glm::max(bigBufferHeight, pair.second->size.y + 2);
		}
		int bigBufferSize = bigBufferWidth * bigBufferHeight * sizeof(unsigned char);
		unsigned char* bigBuffer = new unsigned char[bigBufferSize];
		for (int i = 0; i < bigBufferSize; i++)
		{
			bigBuffer[i] = 0;
		}

		for (pair<int, Character*> pair : this->characters)
		{
			Character* character = pair.second;

			// copy character to the buffer
			for (int row = 0; row < character->size.y; row++)
			{
				int bigBufferRow = character->size.y - row;
				int bigBufferColumn = character->startX;
				int bigBufferStart = bigBufferRow * bigBufferWidth + bigBufferColumn;

				int characterStart = row * character->size.x;

				int copySize = character->size.x;

				if (bigBufferStart + copySize > bigBufferSize)
				{
					cout << "Error";
				}

				memcpy(bigBuffer + bigBufferStart, character->buffer + characterStart, copySize);
			}

			delete character->buffer;

			// store character texture coordinates
			vec2 bigBufferSize = vec2(static_cast<float>(bigBufferWidth), static_cast<float>(bigBufferHeight));
			vec2 texCoord = vec2(static_cast<float>(character->startX), 1.0f) / bigBufferSize;
			vec2 texCoordRelative = static_cast<vec2>(character->size) / bigBufferSize;
			character->texCoords = {
				texCoord,
				texCoord + texCoordRelative,
				texCoord + vec2(0.0f, texCoordRelative.y),

				texCoord,
				texCoord + vec2(texCoordRelative.x, 0.0f),
				texCoord + texCoordRelative
			};
		}

		// load the texture

		this->texture = new Texture();
		this->texture->Bind();
		this->texture->LoadData(bigBufferWidth, bigBufferHeight, GL_RED, GL_RED, GL_UNSIGNED_BYTE, bigBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		Texture::Unbind();

		// free resources

		delete bigBuffer;

		FT_Done_Face(face);
		FT_Done_FreeType(library);

		// initialize VBOs and VAO

		glGenVertexArrays(1, &this->vertexArrayId);
		glBindVertexArray(this->vertexArrayId);

		this->vertices = new VertexBuffer();
		this->vertices->Bind();
		this->vertices->PrepareDynamicData(6 * sizeof(vec2));
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(0);

		this->textureCoordinates = new VertexBuffer();
		this->textureCoordinates->Bind();
		this->textureCoordinates->PrepareDynamicData(6 * sizeof(vec2));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(1);

		VertexBuffer::Unbind();
		glBindVertexArray(0);
	}

	Font::~Font()
	{

	}

	void Font::DrawText(ShaderProgram* shaderProgram, string text, vec2 position, float scale, vec3 colour)
	{
		shaderProgram->Use();
		shaderProgram->SetUniform("textColour", colour);

		glActiveTexture(GL_TEXTURE0);
		this->texture->Bind();
		shaderProgram->SetUniform("textureSampler", 0);

		glBindVertexArray(this->vertexArrayId);

		vec2 actualPosition = position;
		for (string::const_iterator c = text.begin(); c != text.end(); c++)
		{
			int characterCode = *c;
			// if the character is not loaded, skip it
			if (this->characters.find(characterCode) == this->characters.end())
			{
				continue;
			}

			Character* character = this->characters[characterCode];

			// calculate rectangle vertices
			vec2 characterPosition = actualPosition + static_cast<vec2>(character->bearing) * scale;
			vec2 characterSize = static_cast<vec2>(character->size) * scale;

			vector<vec2> vertices = {
				characterPosition,
				characterPosition + characterSize,
				characterPosition + vec2(0.0f, characterSize.y),

				characterPosition,
				characterPosition + vec2(characterSize.x, 0.0f),
				characterPosition + characterSize
			};

			// update vertices VBO
			this->vertices->Bind();
			this->vertices->LoadDataDynamic(vertices);

			// update texture coordinates VBO
			this->textureCoordinates->Bind();
			this->textureCoordinates->LoadDataDynamic(character->texCoords);

			shaderProgram->Use();

			// render rectangle
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// advance actual position (unit of advance is 1/64 pixels)
			actualPosition.x += static_cast<float>(character->advance) / 64.0f * scale;
		}

		VertexBuffer::Unbind();

		glBindVertexArray(0);
		Texture::Unbind();
	}

	vec2 Font::TextSize(string text, float scale)
	{
		vec2 size = vec2(0.0f);

		for (string::const_iterator c = text.begin(); c != text.end(); c++)
		{
			int characterCode = *c;
			// if the character is not loaded, skip it
			if (this->characters.find(characterCode) == this->characters.end())
			{
				continue;
			}

			Character* character = this->characters[characterCode];
			vec2 characterSize = static_cast<vec2>(character->size) * scale;

			size.x += characterSize.x;
			size.y = max(size.y, characterSize.y);
		}

		return size;
	}
}