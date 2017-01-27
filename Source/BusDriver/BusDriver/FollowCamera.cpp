#include "FollowCamera.h"

FollowCamera::FollowCamera(vec3 targetPosition, quat targetRotation)
{
	this->targetPosition = targetPosition;
	this->targetRotation = targetRotation;

	this->UpdateDesiredPosition();

	this->position = this->desiredPosition;
	this->velocity = vec3();

	this->UpdateFront();
}

FollowCamera::~FollowCamera()
{

}

void FollowCamera::FollowTarget(vec3 targetPosition, quat targetRotation, float elapsedTime)
{
	this->targetPosition = targetPosition;
	this->targetRotation = targetRotation;

	this->UpdateDesiredPosition();

	vec3 desiredShift = this->desiredPosition - this->position;
	vec3 acceleration = ACCELERATION_COEFFICIENT * desiredShift;
	this->velocity += acceleration * elapsedTime;
	this->position += this->velocity * elapsedTime;

	this->UpdateFront();
}

void FollowCamera::UpdateDesiredPosition()
{
	// calculate target model matrix
	mat4 modelMatrix;
	modelMatrix = translate(modelMatrix, this->targetPosition);
	modelMatrix *= static_cast<mat4>(this->targetRotation);

	this->desiredPosition = Utility::Transform(DESIRED_POSITION_TARGET_SPACE, inverse(modelMatrix));
}

void FollowCamera::UpdateFront()
{
	vec3 lookingDirection = this->targetPosition - this->position;
	this->UpdateBaseVectors(lookingDirection);
}