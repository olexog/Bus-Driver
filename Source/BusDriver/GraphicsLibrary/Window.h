#pragma once

#include <iostream>
#include <string>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexArray.h"
#include "ColouredVertexArray.h"
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
		void Draw(vector<VertexArray*> vertexArrays);

		///<summary>Determines whether the window received closing events.</summary>
		bool ShouldClose();

		void PollEvents();

		void SwapBuffers();
	private:
		GLFWwindow* glfwWindow;

		ShaderProgram* shaderProgram;
		ShaderProgram* colouredShaderProgram;

		mat4 projection;
		mat4 view;

		void SetViewport();
	};
}