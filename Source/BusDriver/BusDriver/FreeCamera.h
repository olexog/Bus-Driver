#pragma once

#include "Camera.h"

enum class CameraMovement
{
	Up,
	Down,
	Left,
	Right,
	Forward,
	Backward
};

enum class CameraRotation
{
	Up,
	Down,
	Left,
	Right
};

class FreeCamera : public Camera
{
public:
	FreeCamera(vec3 position, float yaw, float pitch);
	~FreeCamera();

	bool GetFastMode();
	void SetFastMode(bool fastModeOn);

	void Move(CameraMovement movement, float elapsedTime);
	void Rotate(CameraRotation rotation, float elapsedTime, bool constrainPitch);
	void RotateWithMouse(float offsetX, float offsetY, bool constrainPitch);
private:
	// The moving velocity in units/sec
	const float VELOCITY = 20.0f;
	// The fast moving velocity in units/sec
	const float FAST_VELOCITY = 100.0f;

	// The turning velocity in degrees/sec
	const float ANGULAR_VELOCITY = 45.0f;
	// The fast turning velocity in degrees/sec
	const float FAST_ANGULAR_VELOCTY = 90.0f;

	// The size of the angle in degrees the camera should turn due to a 1-pixel mouse movement
	const float MOUSE_SENSITIVITY = 0.05f;

	// The amount of rotation around the Y-axis in degrees
	float yaw;
	// The amount of rotation around the X-axis in degrees
	float pitch;

	bool fastModeOn;

	void UpdateFront();

	void ConstrainPitch();
};