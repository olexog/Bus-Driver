#include "Window.h"

namespace GraphicsLibrary
{
	map<GLFWwindow*, Window*> windows = map<GLFWwindow*, Window*>();

	void WindowSize(GLFWwindow* window, int width, int height)
	{
		windows[window]->WindowSizeCallback(window, width, height);
	}

	void Key(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		windows[window]->KeyCallback(window, key, scancode, action, mode);
	}

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
		windows[this->glfwWindow] = this;

		// check the successfulness of the window creation
		if (this->glfwWindow == NULL)
		{
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}

		// select the window
		glfwMakeContextCurrent(this->glfwWindow);

		// callbacks
		glfwSetWindowSizeCallback(this->glfwWindow, WindowSize);
		glfwSetKeyCallback(this->glfwWindow, Key);

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

		this->projection = perspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);

		this->cameraPosition = vec3(0.0f, 3.0f, -100.0f);
		this->cameraDirection = vec3(0.0f, 0.0f, 1.0f);

		glEnable(GL_DEPTH_TEST);		
	}

	Window::~Window()
	{
		windows.erase(this->glfwWindow);

		// terminate the GLFW context
		glfwTerminate();

		delete this->shaderProgram;
		delete this->colouredShaderProgram;
	}

	void Window::Draw(Scene* scene)
	{
		//glClearColor(0.71f, 0.27f, 0.05f, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// move the camera
		float totalTime = static_cast<float>(glfwGetTime());
		float elapsedTime = totalTime - previousTime;
		previousTime = totalTime;

		vec3 up = vec3(0.0f, 1.0f, 0.0f);

		// define camera velocity / angular velocity
		float cameraVelocity = this->pressedKeys[GLFW_KEY_LEFT_SHIFT] ? CAMERA_FAST_VELOCITY : CAMERA_VELOCITY;
		float cameraAngularVelocity = this->pressedKeys[GLFW_KEY_LEFT_SHIFT] ? CAMERA_FAST_ANGULAR_VELOCITY : CAMERA_ANGULAR_VELOCITY;

		// rotate up
		if (this->pressedKeys[GLFW_KEY_I])
		{
			this->cameraDirection = vec3(glm::rotate(mat4(), cameraAngularVelocity * elapsedTime, cross(this->cameraDirection, up)) * vec4(this->cameraDirection, 0.0f));
		}
		// rotate down
		if (this->pressedKeys[GLFW_KEY_K])
		{
			this->cameraDirection = vec3(glm::rotate(mat4(), cameraAngularVelocity * elapsedTime, -cross(this->cameraDirection, up)) * vec4(this->cameraDirection, 0.0f));
		}
		// rotate left
		if (this->pressedKeys[GLFW_KEY_J])
		{
			this->cameraDirection = vec3(glm::rotate(mat4(), cameraAngularVelocity * elapsedTime, up) * vec4(this->cameraDirection, 0.0f));
		}
		// rotate right
		if (this->pressedKeys[GLFW_KEY_L])
		{
			this->cameraDirection = vec3(glm::rotate(mat4(), cameraAngularVelocity * elapsedTime, -up) * vec4(this->cameraDirection, 0.0f));
		}
		// translate up
		if (this->pressedKeys[GLFW_KEY_W])
		{
			this->cameraPosition += cameraVelocity * elapsedTime * up;
		}
		// translate down
		if (this->pressedKeys[GLFW_KEY_S])
		{
			this->cameraPosition -= cameraVelocity * elapsedTime * up;
		}
		// translate left
		if (this->pressedKeys[GLFW_KEY_A])
		{
			this->cameraPosition -= cameraVelocity * elapsedTime * cross(this->cameraDirection, up);
		}
		// translate right
		if (this->pressedKeys[GLFW_KEY_D])
		{
			this->cameraPosition += cameraVelocity * elapsedTime * cross(this->cameraDirection, up);
		}
		// translate forward
		if (this->pressedKeys[GLFW_KEY_E])
		{
			this->cameraPosition += cameraVelocity * elapsedTime * this->cameraDirection;
		}
		// translate backward
		if (this->pressedKeys[GLFW_KEY_Q])
		{
			this->cameraPosition -= cameraVelocity * elapsedTime * this->cameraDirection;
		}

		// calculate the view
		this->view = lookAt(this->cameraPosition, this->cameraPosition + this->cameraDirection, vec3(0.0f, 1.0f, 0.0f));

		// set the uniforms
		this->shaderProgram->SetUniform("projection", this->projection);
		this->shaderProgram->SetUniform("view", this->view);

		vec3 lightPosition = vec3(0.0f, 200.0f, 1000.0f);
		//vec3 lightColour = vec3(0.71f, 0.27f, 0.05f);
		vec3 lightColour = vec3(1.0f);

		this->colouredShaderProgram->SetUniform("projection", this->projection);
		this->colouredShaderProgram->SetUniform("view", this->view);
		this->colouredShaderProgram->SetUniform("lightPosition", lightPosition);
		this->colouredShaderProgram->SetUniform("lightColour", lightColour);

		// draw the models
		for (PositionedModel* positionedModel : scene->models)
		{
			mat4 modelMatrix;
			modelMatrix = translate(modelMatrix, positionedModel->position);
			modelMatrix *= static_cast<mat4>(positionedModel->rotation);
			this->colouredShaderProgram->SetUniform("model", modelMatrix);
			positionedModel->model->Draw(this->shaderProgram, this->colouredShaderProgram);
		}
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

	void Window::WindowSizeCallback(GLFWwindow* window, int width, int height)
	{
		this->SetViewport();
	}

	void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(this->glfwWindow, GL_TRUE);
		}
		else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (key == GLFW_KEY_M && action == GLFW_RELEASE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (action == GLFW_PRESS)
		{
			this->pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			this->pressedKeys[key] = false;
		}
	}

	float Window::GetElapsedTime()
	{
		float totalTime = static_cast<float>(glfwGetTime());
		float elapsedTime = totalTime - this->previousTime;
		return elapsedTime;
	}
}