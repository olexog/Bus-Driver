#version 330 core

in vec2 textureCoordinate;

out vec4 colour;

uniform sampler2D textureSampler;

void main()
{
	colour = texture(textureSampler, textureCoordinate);
}