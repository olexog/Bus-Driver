#include "FreeCamera.h"

FreeCamera::FreeCamera(vec3 position, float yaw, float pitch)
{
	this->position = position;
	this->yaw = yaw;
	this->pitch = pitch;

	this->UpdateFront();
}

FreeCamera::~FreeCamera()
{

}

bool FreeCamera::GetFastMode()
{
	return this->fastModeOn;
}

void FreeCamera::SetFastMode(bool fastModeOn)
{
	this->fastModeOn = fastModeOn;
}

void FreeCamera::Move(CameraMovement movement, float elapsedTime)
{
	float velocity = this->fastModeOn ? FAST_VELOCITY : VELOCITY;
	float shift = velocity * elapsedTime;

	switch (movement)
	{
	case CameraMovement::Up:
		this->position += shift * this->GetUp();
		break;
	case CameraMovement::Down:
		this->position -= shift * this->GetUp();
		break;
	case CameraMovement::Right:
		this->position += shift * this->GetRight();
		break;
	case CameraMovement::Left:
		this->position -= shift * this->GetRight();
		break;
	case CameraMovement::Forward:
		this->position += shift * this->GetFront();
		break;
	case CameraMovement::Backward:
		this->position -= shift * this->GetFront();
		break;
	}
}

void FreeCamera::Rotate(CameraRotation rotation, float elapsedTime, bool constrainPitch)
{
	float angularVelocity = this->fastModeOn ? FAST_ANGULAR_VELOCTY : ANGULAR_VELOCITY;
	float turningAngle = angularVelocity * elapsedTime;

	switch (rotation)
	{
	case CameraRotation::Up:
		this->pitch += turningAngle;
		break;
	case CameraRotation::Down:
		this->pitch -= turningAngle;
		break;
	case CameraRotation::Left:
		this->yaw += turningAngle;
		break;
	case CameraRotation::Right:
		this->yaw -= turningAngle;
		break;
	}

	if (constrainPitch)
	{
		this->ConstrainPitch();
	}

	this->UpdateFront();
}

void FreeCamera::RotateWithMouse(float offsetX, float offsetY, bool constrainPitch)
{
	this->yaw += offsetX * MOUSE_SENSITIVITY;
	this->pitch -= offsetY * MOUSE_SENSITIVITY;

	if (constrainPitch)
	{
		this->ConstrainPitch();
	}

	this->UpdateFront();
}

void FreeCamera::UpdateFront()
{
	vec3 front;
	front.x = cos(radians(this->yaw)) * cos(radians(this->pitch));
	front.y = sin(radians(this->pitch));
	front.z = sin(radians(this->yaw)) * cos(radians(this->pitch));
	front = normalize(front);

	this->UpdateBaseVectors(front);
}

void FreeCamera::ConstrainPitch()
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