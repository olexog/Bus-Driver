#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Alignment.h"

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	class TextDrawing
	{
	public:
		TextDrawing(string text, float scale, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, vec2 margin, vec3 colour);
		~TextDrawing();

		string Text;
		float Scale;
		HorizontalAlignment HorizontalAlignment;
		VerticalAlignment VerticalAlignment;
		vec2 Margin;
		vec3 Colour;
	};
}