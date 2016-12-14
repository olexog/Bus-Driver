#version 330 core
layout (location = 0) in vec3 isStartPoint;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 startPoint;
uniform vec3 endPoint;

void main()
{
	vec3 position;
	if (isStartPoint.x == 1.0)
	{
		position = startPoint;
	}
	else
	{
		position = endPoint;
	}
	
	gl_Position = projection * view * vec4(position, 1.0);
}