#version 330 core

in vec2 textureCoordinate;

out vec4 colour;

uniform sampler2D textureSampler;

void main()
{
	float c = texture(textureSampler, textureCoordinate).r;
	colour = vec4(c, c, c, 1.0);
}