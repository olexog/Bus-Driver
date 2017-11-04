#include "TextDrawing.h"

namespace GraphicsLibrary
{
	TextDrawing::TextDrawing(string text, float scale, GraphicsLibrary::HorizontalAlignment horizontalAlignment, GraphicsLibrary::VerticalAlignment verticalAlignment, vec2 margin, vec3 colour)
	{
		this->Text = text;
		this->Scale = scale;
		this->HorizontalAlignment = horizontalAlignment;
		this->VerticalAlignment = verticalAlignment;
		this->Margin = margin;
		this->Colour = colour;
	}

	TextDrawing::~TextDrawing()
	{

	}
}