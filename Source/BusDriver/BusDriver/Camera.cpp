#include "Camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

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

void Camera::UpdateBaseVectors(vec3 front)
{
	this->front = normalize(front);
	this->right = normalize(cross(this->front, WORLD_UP));
	this->up = normalize(cross(this->right, this->front));
}