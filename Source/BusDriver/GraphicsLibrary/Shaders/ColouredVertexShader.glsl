#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 ambientColour;
layout (location = 3) in vec3 diffuseColour;
layout (location = 4) in vec3 specularColour;

out vec3 normal;
out vec3 ambient;
out vec3 diffuse;
out vec3 specular;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * vec4(vertexPosition, 1.0);
	
	normal = vertexNormal;
	ambient = ambientColour;
	diffuse = diffuseColour;
	specular = specularColour;
}