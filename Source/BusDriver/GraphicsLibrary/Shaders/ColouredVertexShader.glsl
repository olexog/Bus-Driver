#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colour;

out vec3 outColour;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * vec4(position, 1.0);
	outColour = colour;
}