#include "OpenGl.h"

namespace GraphicsLibrary
{
	OpenGl::OpenGl(int width, int height)
	{
		// initialize GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "GLEW has not happened to initialize successfully." << std::endl;
		}

		// get the default error
		glGetError();

		// set the starting context's size
		this->SetViewport(width, height);

		// load shader programs
		this->shaderProgram = new ShaderProgram("Shaders\\VertexShader.glsl", "Shaders\\FragmentShader.glsl");
		this->colouredShaderProgram = new ShaderProgram("Shaders\\ColouredVertexShader.glsl", "Shaders\\ColouredFragmentShader.glsl");

		// enable the depth buffer for depth testing
		glEnable(GL_DEPTH_TEST);		
	}

	OpenGl::~OpenGl()
	{
		delete this->shaderProgram;
		delete this->colouredShaderProgram;
	}

	void OpenGl::Draw(Scene* scene)
	{
		glClearColor(0.71f, 0.27f, 0.05f, 0);
		//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
			modelMatrix = translate(modelMatrix, *positionedModel->GetPosition());
			modelMatrix *= static_cast<mat4>(*positionedModel->GetOrientation());
			this->colouredShaderProgram->SetUniform("model", modelMatrix);
			positionedModel->GetModel()->Draw(this->shaderProgram, this->colouredShaderProgram);
		}
	}

	void OpenGl::SetViewport(int width, int height)
	{
		// set where to draw on the context
		glViewport(0, 0, width, height);

		// calculate the projection matrix
		this->projection = perspective(45.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
	}

	void OpenGl::SetWireframeMode(bool wireframeMode)
	{
		this->wireframeMode = wireframeMode;
	}

	void OpenGl::SetCamera(vec3 position, vec3 direction)
	{
		this->cameraPosition = position;
		this->cameraDirection = direction;
	}
}