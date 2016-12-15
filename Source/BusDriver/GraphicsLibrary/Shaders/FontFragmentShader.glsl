#version 330 core

in vec2 textureCoordinate;

out vec4 colour;

uniform sampler2D textureSampler;
uniform vec3 textColour;

void main()
{
	float alpha = texture(textureSampler, textureCoordinate).r;
	colour = vec4(textColour, alpha);
	//colour = vec4(1.0, 1.0, 1.0, 0.0);
}