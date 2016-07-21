#version 330 core

in vec3 outColour;

out vec4 colour;

void main()
{
	colour = vec4(outColour, 1.0);
}