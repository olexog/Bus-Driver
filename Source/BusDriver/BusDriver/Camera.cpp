#include "Camera.h"

Camera::Camera(vec3 position, float yaw, float pitch)
{
	this->mode = CameraMode::Static;

	this->position = position;
	this->yaw = yaw;
	this->pitch = pitch;

	this->UpdateBaseVectors();
}

Camera::Camera(vec3 targetPosition, quat targetRotation)
{
	this->mode = CameraMode::FollowTarget;

	this->UpdateBaseVectors();
}

Camera::~Camera()
{
}

CameraMode Camera::GetMode()
{
	return this->mode;
}

void Camera::SetMode(CameraMode mode)
{
	this->mode = mode;
}

vec3 Camera::GetPosition()
{
	return this->position;
}

vec3 Camera::GetFront()
{
	return this->front;
}

vec3 Camera::GetUp()
{
	return this->up;
}

vec3 Camera::GetRight()
{
	return this->right;
}

void Camera::SetFastMode(bool fastMode)
{
	this->fastMode = fastMode;
}

void Camera::Move(CameraMovement movement, float elapsedTime)
{
	if (this->mode != CameraMode::Static)
	{
		return;
	}

	float velocity = this->fastMode ? FAST_VELOCITY : VELOCITY;

	switch (movement)
	{
	case CameraMovement::Up:
		this->position += velocity * elapsedTime * this->up;
		break;
	case CameraMovement::Down:
		this->position -= velocity * elapsedTime * this->up;
		break;
	case CameraMovement::Right:
		this->position += velocity * elapsedTime * this->right;
		break;
	case CameraMovement::Left:
		this->position -= velocity * elapsedTime * this->right;
		break;
	case CameraMovement::Forward:
		this->position += velocity * elapsedTime * this->front;
		break;
	case CameraMovement::Backward:
		this->position -= velocity * elapsedTime * this->front;
		break;
	}
}

void Camera::Rotate(float offsetX, float offsetY, bool constrainPitch)
{
	if (this->mode != CameraMode::Static)
	{
		return;
	}

	offsetX *= MOUSE_SENSITIVITY;
	offsetY *= MOUSE_SENSITIVITY;

	this->yaw += offsetX;
	this->pitch += offsetY;

	if (constrainPitch)
	{
		if (this->pitch > 89.0f)
		{
			this->pitch = 89.0f;
		}
		if (this->pitch < -89.0f)
		{
			this->pitch = -89.0f;
		}
	}
}

void Camera::SetTargetTransformation(vec3 targetPosition, quat targetRotation)
{
	this->targetPosition = targetPosition;
	this->targetRotation = targetRotation;
}

void Camera::UpdateBaseVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	front.y = sin(glm::radians(this->pitch));
	front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->front = glm::normalize(front);

	// Also re-calculate the Right and Up vector
	this->right = glm::normalize(glm::cross(this->front, WORLD_UP));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->up = glm::normalize(glm::cross(this->right, this->front));
}