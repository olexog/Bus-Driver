#pragma once

#include <iostream>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Model.h"
#include "ShaderProgram.h"

using namespace std;

namespace GraphicsLibrary
{
	///<summary>Represents an OpenGL context to be drawn on.</summary>
	class Window
	{
	public:
		Window(int width, int height, string caption);
		~Window();

		///<summary>Draws to the context.</summary>
		void Draw(Model* model);

		///<summary>Determines whether the window received closing events.</summary>
		bool ShouldClose();

		void PollEvents();

		void SwapBuffers();

		ShaderProgram* GetShaderProgram();
	private:
		GLFWwindow* glfwWindow;

		ShaderProgram* shaderProgram;

		void SetViewport();

		PxMat44 CreatePerspective(float fovy, float aspect, float zNear, float zFar);

		PxMat44 projectionMatrix;
	};
}