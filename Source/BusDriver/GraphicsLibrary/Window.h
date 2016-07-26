#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Scene.h"
#include "ShaderProgram.h"

using namespace std;

using namespace glm;

namespace GraphicsLibrary
{
	///<summary>Represents an OpenGL context to be drawn on.</summary>
	class Window
	{
	public:
		Window(int width, int height, string caption);
		~Window();

		///<summary>Draws to the context.</summary>
		void Draw(Scene* scene, vec3 vehiclePosition, quat vehicleRotation, vector<Model*> physicsWheelModels, vector<vec3> positions, vector<quat> rotations, Model* physicsChassisModel, vec3 chassisPosition, quat chassisRotation, Model* wheelModel, Model* chassisModel);

		///<summary>Determines whether the window received closing events.</summary>
		bool ShouldClose();

		void PollEvents();

		void SwapBuffers();

		void WindowSizeCallback(GLFWwindow* window, int width, int height);

		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

		bool IsPressed(int key);

		float GetElapsedTime();
	private:
		GLFWwindow* glfwWindow;

		bool pressedKeys[GLFW_KEY_LAST];

		ShaderProgram* shaderProgram;
		ShaderProgram* colouredShaderProgram;

		float previousTime;

		const float CAMERA_VELOCITY = 20.0f;
		const float CAMERA_ANGULAR_VELOCITY = 0.5f;
		const float CAMERA_FAST_VELOCITY = 100.0f;
		const float CAMERA_FAST_ANGULAR_VELOCITY = 1.0f;

		vec3 cameraPosition;
		vec3 cameraDirection;

		mat4 projection;
		mat4 view;

		void SetViewport();
	};
}