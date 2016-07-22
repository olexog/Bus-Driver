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
		void Draw(Scene* scene);

		///<summary>Determines whether the window received closing events.</summary>
		bool ShouldClose();

		void PollEvents();

		void SwapBuffers();

		void WindowSizeCallback(GLFWwindow* window, int width, int height);

		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	private:
		GLFWwindow* glfwWindow;

		bool pressedKeys[GLFW_KEY_LAST];

		ShaderProgram* shaderProgram;
		ShaderProgram* colouredShaderProgram;

		float prevoiusTime;

		const float CAMERA_VELOCITY = 100.0f;
		const float CAMERA_ANGULAR_VELOCITY = 1.0f;

		vec3 cameraPosition;
		vec3 cameraDirection;

		mat4 projection;
		mat4 view;

		void SetViewport();
	};
}