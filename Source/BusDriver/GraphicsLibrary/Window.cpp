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

		this->projectionMatrix = this->CreatePerspective(PxHalfPi, static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
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
		glClear(GL_COLOR_BUFFER_BIT);

		model->Draw();
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

	ShaderProgram* Window::GetShaderProgram()
	{
		return this->shaderProgram;
	}

	PxMat44 Window::CreatePerspective(float fovy, float aspect, float zNear, float zFar)
	{
		PxMat44 result;

		float tanHalfFovy = tan(fovy / 2);

		result[0][0] = 1 / (aspect * tanHalfFovy);
		result[1][1] = 1 / (tanHalfFovy);
		result[2][2] = -(zFar + zNear) / (zFar - zNear);
		result[2][3] = -1;
		result[3][2] = -(2 * zFar * zNear) / (zFar - zNear);

		return result;
	}
}