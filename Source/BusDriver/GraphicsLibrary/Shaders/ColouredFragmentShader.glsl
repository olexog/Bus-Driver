#version 330 core

in vec3 position;
in vec3 normal;
in vec3 ambient;
in vec3 diffuse;
in vec3 specular;

out vec4 colour;

uniform vec3 lightPosition;
uniform vec3 lightColour;

void main()
{
	// Ambient light
	float ambientStrength = 0.1;
	
	// Diffuse light
	vec3 light = normalize(lightPosition - position);
	float diffuseStrength = max(dot(normal, light), 0.0);
	
	// Calculating final colour
	colour = vec4(lightColour * (ambientStrength * ambient + diffuseStrength * diffuse), 1.0);
	//colour = vec4(normal, 1.0);
}