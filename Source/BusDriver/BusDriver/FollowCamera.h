#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "GraphicsUtility.h"

using namespace GraphicsLibrary;

class FollowCamera : public Camera
{
public:
	FollowCamera(vec3 targetPosition, quat targetRotation);
	~FollowCamera();

	void FollowTarget(vec3 targetPosition, quat targetRotation, float elapsedTime);
private:
	const vec3 DESIRED_POSITION_TARGET_SPACE = vec3(0.0f, 5.0f, -15.0f);
	const float FOLLOW_COEFFICIENT = 8.0f;

	vec3 targetPosition;
	quat targetRotation;

	vec3 desiredPosition;

	void UpdateDesiredPosition();

	void UpdateFront();
};

