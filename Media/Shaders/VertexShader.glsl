#version 330 core

uniform mat4 projectionMatrix;

void main()
{
	gl_Position = vec4(position, 1.0);
}