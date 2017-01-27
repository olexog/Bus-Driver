#include "FollowCamera.h"

FollowCamera::FollowCamera(vec3 targetPosition, quat targetRotation)
{
	this->targetPosition = targetPosition;
	this->targetRotation = targetRotation;

	this->UpdateDesiredPosition();

	this->position = this->desiredPosition;

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

	this->position = (this->position - this->desiredPosition) * pow(2.0f, - FOLLOW_COEFFICIENT * elapsedTime) + this->desiredPosition;

	this->UpdateFront();
}

void FollowCamera::UpdateDesiredPosition()
{
	// calculate target model matrix
	mat4 modelMatrix;
	modelMatrix = translate(modelMatrix, this->targetPosition);
	modelMatrix *= static_cast<mat4>(this->targetRotation);

	this->desiredPosition = GraphicsUtility::Transform(DESIRED_POSITION_TARGET_SPACE, modelMatrix);
}

void FollowCamera::UpdateFront()
{
	vec3 lookingDirection = this->targetPosition - this->position;
	this->UpdateBaseVectors(lookingDirection);
}