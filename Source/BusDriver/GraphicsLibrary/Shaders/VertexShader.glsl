#version 330 core

const int CASCADE_COUNT = 3;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;

out vec3 position;
out vec3 normal;
out vec2 textureCoordinate;
out vec4 positionsLightSpace[CASCADE_COUNT];
out float depthToCamera;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 lightTransforms[CASCADE_COUNT];

void main()
{
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0);

	depthToCamera = gl_Position.z;
	
	position = (model * vec4(vertexPosition, 1.0)).xyz;
	//normal = normalize((model * vec4(vertexNormal, 1.0)).xyz);
	normal = mat3(transpose(inverse(model))) * vertexNormal;
	textureCoordinate = texCoord;
	
	for (int i = 0; i < CASCADE_COUNT; i++)
	{
		positionsLightSpace[i] = lightTransforms[i] * vec4(position, 1.0);
	}
}