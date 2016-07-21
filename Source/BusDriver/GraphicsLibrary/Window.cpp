#include "Window.h"

namespace GraphicsLibrary
{
	Window::Window(int width, int height, string caption)
	{
		// initialize GLFW
		glfwInit();

		// assert OpenGL versions
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// create window
		this->glfwWindow = glfwCreateWindow(width, height, caption.c_str(), NULL, NULL);

		// check the successfulness of the window creation
		if (this->glfwWindow == NULL)
		{
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}

		// select the window
		glfwMakeContextCurrent(this->glfwWindow);

		// initialize GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "GLEW has not happened to initialize successfully." << std::endl;
		}

		glGetError();

		this->SetViewport();

		this->shaderProgram = new ShaderProgram("Shaders\\VertexShader.glsl", "Shaders\\FragmentShader.glsl");
		this->colouredShaderProgram = new ShaderProgram("Shaders\\ColouredVertexShader.glsl", "Shaders\\ColouredFragmentShader.glsl");

		this->projection = perspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);

		this->view = lookAt(vec3(-10.0f, 5.0f, -5.0f), vec3(0, 0, 0), vec3(0, 1, 0));

		glEnable(GL_DEPTH_TEST);
	}

	Window::~Window()
	{
		// terminate the GLFW context
		glfwTerminate();

		delete this->shaderProgram;
	}

	void Window::Draw(Model* model)
	{
		glClearColor(0, 0, 0.7f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->shaderProgram->SetUniform("projection", this->projection);
		this->shaderProgram->SetUniform("view", this->view);

		vec3 lightPosition = vec3(0.0f, 1.0f, -3.0f);

		this->colouredShaderProgram->SetUniform("projection", this->projection);
		this->colouredShaderProgram->SetUniform("view", this->view);
		this->colouredShaderProgram->SetUniform("lightPosition", lightPosition);

		model->Draw(this->shaderProgram, this->colouredShaderProgram);
	}

	bool Window::ShouldClose()
	{
		// determine whether the window should close
		return glfwWindowShouldClose(this->glfwWindow) == GL_TRUE;
	}

	void Window::SetViewport()
	{
		// get the screen width and height
		int width, height;
		glfwGetFramebufferSize(this->glfwWindow, &width, &height);

		// set the width and height
		glViewport(0, 0, width, height);
	}

	void Window::PollEvents()
	{
		// poll events from the window
		glfwPollEvents();
	}

	void Window::SwapBuffers()
	{
		// swap the screen buffers
		glfwSwapBuffers(this->glfwWindow);
	}
}