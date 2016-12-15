#include "Font.h"

namespace GraphicsLibrary
{
	Font::Font(string fontPath, int size, int firstCharacter, int lastCharacter)
	{
		// initialize FreeType library

		FT_Error result = FT_Init_FreeType(&this->library);
		if (result)
		{
			cout << "FreeType library initialization has failed. Error code: " << result << endl;
			return;
		}

		// load face

		result = FT_New_Face(Font::library, fontPath.c_str(), 0, &this->face);
		if (result)
		{
			cout << "Face creation has failed. Error code: " << result << endl;
			return;
		}

		FT_Set_Pixel_Sizes(this->face, 0, size);

		// setup necessary OpenGL stuff: alignment and blending

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// iterate and load each character

		int startX = 0;
		this->characters = map<int, Character>();

		for (int c = firstCharacter; c <= lastCharacter; c++)
		{
			result = FT_Load_Char(this->face, c, FT_LOAD_RENDER);
			if (result)
			{
				cout << "Failed to load character with code " << c << ". Error code: " << result << endl;
			}

			Character character = {
				startX,
				ivec2(this->face->glyph->bitmap.width, this->face->glyph->bitmap.rows),
				ivec2(this->face->glyph->bitmap_left, this->face->glyph->bitmap_top - this->face->glyph->bitmap.rows),
				this->face->glyph->advance.x
			};

			startX += character.size.x;

			int bufferSize = character.size.x * character.size.y * sizeof(unsigned char);
			unsigned char* buffer = new unsigned char[bufferSize];
			memcpy(buffer, this->face->glyph->bitmap.buffer, bufferSize);
			character.buffer = buffer;

			this->characters[c] = character;
		}

		// generate a buffer containing all characters in the font

		int bufferWidth = 0;
		int bufferHeight = 0;
		for (pair<int, Character> pair : this->characters)
		{
			bufferWidth += pair.second.size.x;
			bufferHeight = glm::max(bufferHeight, pair.second.size.y);
		}
		int bufferSize = bufferWidth * bufferHeight * sizeof(unsigned char);
		unsigned char* buffer = new unsigned char[bufferSize];

		for (pair<int, Character> pair : this->characters)
		{
			Character character = pair.second;

			// copy character to the buffer
			for (int row = 0; row < character.size.y; row++)
			{
				int bufferRow = character.size.y - 1 - row;
				int bufferColumn = character.startX;
				int bufferStart = bufferRow * bufferWidth + bufferColumn;

				int characterStart = row * character.size.x;

				int copySize = character.size.x;

				memcpy(buffer + bufferStart, character.buffer + characterStart, copySize);
			}

			// store character texture coordinates
			vec2 texCoord = vec2(static_cast<float>(character.startX) / static_cast<float>(bufferWidth), 0.0f);
			vec2 texCoordRelative = static_cast<vec2>(character.size) / vec2(static_cast<float>(bufferWidth), static_cast<float>(bufferHeight));
			character.texCoords = vector<vec2>({
				texCoord,
				texCoord + texCoordRelative,
				texCoord + vec2(0.0f, texCoordRelative.y),

				texCoord,
				texCoord + vec2(texCoordRelative.x, 0.0f),
				texCoord + texCoordRelative
			});

			delete character.buffer;
		}

		// load the texture

		this->texture = new Texture();
		this->texture->Bind();
		this->texture->LoadData(bufferWidth, bufferHeight, GL_RED, GL_RED, GL_UNSIGNED_BYTE, buffer);
		Texture::Unbind();

		// free resources

		delete buffer;

		FT_Done_Face(this->face);
		FT_Done_FreeType(this->library);

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

			Character character = this->characters[characterCode];

			// calculate rectangle vertices
			vec2 characterPosition = actualPosition + static_cast<vec2>(character.bearing) * scale;
			vec2 characterSize = static_cast<vec2>(character.size) * scale;

			vector<vec2> vertices = vector<vec2>({
				characterPosition,
				characterPosition + characterSize,
				characterPosition + vec2(0.0f, characterSize.y),

				characterPosition,
				characterPosition + vec2(characterSize.x, 0.0f),
				characterPosition + characterSize
			});

			// update vertices VBO
			this->vertices->Bind();
			this->vertices->LoadDataDynamic(vertices);

			// update texture coordinates VBO
			this->textureCoordinates->Bind();
			this->textureCoordinates->LoadDataDynamic(character.texCoords);

			// render rectangle
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// advance actual position (unit of advance is 1/64 pixels)
			actualPosition.x += static_cast<float>(character.advance) / 64.0f * scale;
		}

		VertexBuffer::Unbind();

		glBindVertexArray(0);
		Texture::Unbind();
	}
}