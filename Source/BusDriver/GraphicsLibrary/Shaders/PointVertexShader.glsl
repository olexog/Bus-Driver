#version 330 core
layout (location = 0) in vec3 vertexPosition;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 position;
uniform float pointSize;

void main()
{
	gl_Position = projection * view * vec4(position, 1.0);
	gl_PointSize = pointSize;
}