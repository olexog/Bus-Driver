#version 330 core

const int CASCADE_COUNT = 3;

in vec3 position;
in vec3 normal;
in vec2 textureCoordinate;
in vec4 positionsLightSpace[CASCADE_COUNT];;
in float depthToCamera;

out vec4 colour;

uniform vec3 lightPosition;
uniform vec3 lightColour;

uniform float cascadeEnds[CASCADE_COUNT + 1];

uniform sampler2D textureSampler;
uniform sampler2D shadowMaps[CASCADE_COUNT];

uniform float shadowsOn;

//uniform int cascadeNumber;

// Returns 1 if the fragment is in shadow, otherwise 0
float IsInShadow(vec4 positionLightSpace, float angleOfIncidence, int cascadeNumber)
{
	// Perspective divison will clip the position into [-1; 1] resulting Normalised Device Coordinates
	vec3 positionLightSpaceNDC = positionLightSpace.xyz / positionLightSpace.w;
	// Clip it into [0; 1] since depth range is the same
	positionLightSpaceNDC = positionLightSpaceNDC * 0.5 + 0.5;

	// The distance between the light and this fragment
	float fragmentDepth = positionLightSpaceNDC.z;
	
	// The inaccuracy of bitmap image cause shadow acne therefore a bias introduced when a fragment
	// being that farther from the light than the obstacle will also be lit
	float bias = max(0.05 * (1.0 - angleOfIncidence), 0.005); 

	float inShadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMaps[cascadeNumber], 0);
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <=1; y++)
		{
			// The distance between the light and the lit obstacle, which can be seen from the light's viewport
			float obstacleDepth = texture(shadowMaps[cascadeNumber], positionLightSpaceNDC.xy + vec2(x, y) * texelSize).r;

			// Determine whether the fragment is farther to the light than the lit obstacle meaning that it is in shadow
			inShadow += fragmentDepth - bias > obstacleDepth ? 1.0 : 0.0;
		}
	}

	return inShadow / 9.0;
}

void main()
{
	vec3 textureColour = texture(textureSampler, textureCoordinate).rgb;
	//textureColour = positionLightSpace.xyz / 100.0;

	// Ambient light
	float ambientStrength = 0.1;
	
	// Calculate the light's angle of incidence
	vec3 lightDirection = normalize(lightPosition - position);
	float angleOfIncidence = dot(normal, lightDirection);

	int cascadeNumber = 0;
	
	for (int i = 0; i < CASCADE_COUNT; i++)
	{
		if (depthToCamera < cascadeEnds[i + 1])
		{
			cascadeNumber = i;
			break;
		}
	}
	
	// Shadow checking
	float inShadow = IsInShadow(positionsLightSpace[cascadeNumber], angleOfIncidence, cascadeNumber);
	inShadow = min(inShadow, shadowsOn);
	
	//if (shadowsOn == 1.0)
	if (false)
	{
		if (cascadeNumber == 0)
		{
			textureColour = vec3(0.0, 0.0, 1.0);
		}
		else if (cascadeNumber == 1)
		{
			textureColour = vec3(0.0, 1.0, 0.0);
		}
		else
		{
			textureColour = vec3(1.0, 0.0, 0.0);
		}
	}
	
	// Diffuse light
	float diffuseStrength = max(angleOfIncidence, 0.0);

	// Null it if it is in shadow
	diffuseStrength *= 1 - inShadow;
	
	// Calculating final colour
	colour = vec4(lightColour * textureColour * (ambientStrength + diffuseStrength), 1.0);
	//colour = vec4(depthToCamera * 0.01);
}