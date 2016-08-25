#version 330 core

in vec3 position;
in vec3 normal;
in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in vec4 positionLightSpace;

out vec4 colour;

uniform vec3 lightPosition;
uniform vec3 lightColour;

uniform sampler2D shadowMap;

// Returns 1 if the fragment is in shadow, otherwise 0
float IsInShadow(vec4 positionLightSpace, float angleOfIncidence)
{
	// Perspective divison will clip the position into [-1; 1] resulting Normalised Device Coordinates
	vec3 positionLightSpaceNDC = positionLightSpace.xyz / positionLightSpace.w;
	// Clip it into [0; 1] since depth range is the same
	positionLightSpaceNDC = positionLightSpaceNDC * 0.5 + 0.5;

	// The distance between the light and the lit obstacle, which can be seen from the light's viewport
	float obstacleDepth = texture(shadowMap, positionLightSpaceNDC.xy).r;

	// The distance between the light and this fragment
	float fragmentDepth = positionLightSpaceNDC.z;
	
	// The inaccuracy of bitmap image cause shadow acne therefore a bias introduced when a fragment
	// being that farther from the light than the obstacle will also be lit
	float bias = max(0.05 * (1.0 - angleOfIncidence), 0.005); 

	// Determine whether the fragment is farther to the light than the lit obstacle meaning that it is in shadow
	float inShadow = fragmentDepth - bias > obstacleDepth ? 1.0 : 0.0;

	return inShadow;
}

void main()
{
	// Ambient light
	float ambientStrength = 0.1;
	
	// Calculate the light's angle of incidence
	vec3 lightDirection = normalize(lightPosition - position);
	float angleOfIncidence = dot(normal, lightDirection);

	// Shadow checking
	float inShadow = IsInShadow(positionLightSpace, angleOfIncidence);
	
	// Diffuse light
	float diffuseStrength = max(angleOfIncidence, 0.0);

	// Null it if it is in shadow
	diffuseStrength *= 1 - inShadow;
	
	// Calculating final colour
	colour = vec4(lightColour * (ambientStrength * ambient + diffuseStrength * diffuse), 1.0);
}