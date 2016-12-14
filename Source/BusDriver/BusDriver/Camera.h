#pragma once

#include <glm\vec3.hpp>
#include <glm\gtc\quaternion.hpp>

using namespace glm;

enum class CameraMode
{
	Static,
	FollowTarget
};

enum class CameraMovement
{
	Up,
	Down,
	Left,
	Right,
	Forward,
	Backward
};

class Camera
{
public:
	Camera(vec3 position, float yaw, float pitch);
	Camera(vec3 targetPosition, quat targetRotation);
	~Camera();

	CameraMode GetMode();
	void SetMode(CameraMode mode);

	vec3 GetPosition();
	vec3 GetFront();
	vec3 GetUp();
	vec3 GetRight();

	void SetFastMode(bool fastMode);

	void Move(CameraMovement movement, float elapsedTime);
	void Rotate(float offsetX, float offsetY, bool constrainPitch);

	void SetTargetTransformation(vec3 targetPosition, quat targetRotation);
private:
	const float VELOCITY = 20.0f;
	const float FAST_VELOCITY = 100.0f;

	const float MOUSE_SENSITIVITY = 0.25f;

	const vec3 WORLD_UP = vec3(0.0f, 1.0f, 0.0f);

	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;

	float yaw;
	float pitch;

	bool fastMode;

	vec3 targetPosition;
	quat targetRotation;

	CameraMode mode;

	void UpdateBaseVectors();
};