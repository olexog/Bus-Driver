#pragma once

#include <glm\vec3.hpp>
#include <glm\gtc\quaternion.hpp>

using namespace glm;

class Camera
{
public:
	Camera();
	~Camera();

	vec3 GetPosition();
	vec3 GetFront();
	vec3 GetUp();
	vec3 GetRight();
protected:
	const vec3 WORLD_UP = vec3(0.0f, 1.0f, 0.0f);

	vec3 position;

	void UpdateBaseVectors(vec3 front);
private:
	vec3 front;
	vec3 up;
	vec3 right;
};