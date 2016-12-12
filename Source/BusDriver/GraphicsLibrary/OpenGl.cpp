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
		this->depthShaderProgram = new ShaderProgram("Shaders\\DepthVertexShader.glsl", "Shaders\\DepthFragmentShader.glsl");

		// enable the depth buffer for depth testing
		glEnable(GL_DEPTH_TEST);

		// initialize depth map
		this->depthMapBuffer = new FrameBuffer();
		this->depthMapTexture = new Texture();
		this->depthMapTexture->Bind();
		this->depthMapTexture->LoadData(this->SHADOW_MAP_WIDTH, this->SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// clamping to the border needed to ignore the areas which is not covered by the shadow map
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		this->depthMapBuffer->Bind();
		this->depthMapTexture->AttachToFramebuffer(GL_DEPTH_ATTACHMENT);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		FrameBuffer::Unbind();

		vector<char> data;
		data.push_back(255);
		data.push_back(0);
		data.push_back(0);

		data.push_back(0);
		data.push_back(255);
		data.push_back(0);
		this->texture = new Texture();
		this->texture->Bind();
		this->texture->LoadData(2, 1, data.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		Texture::Unbind();
		//this->texture = Utility::LoadTexture("Models\\newspaper.png");
	}

	OpenGl::~OpenGl()
	{
		delete this->shaderProgram;
		delete this->depthShaderProgram;
		delete this->depthMapTexture;
		delete this->depthMapBuffer;
	}

	void OpenGl::Draw(Scene* scene)
	{
		// calculate view matrices
		this->view = lookAt(this->cameraPosition, this->cameraPosition + this->cameraDirection, vec3(0.0f, 1.0f, 0.0f));

		vec3 lightDirection = vec3(-1, 0, 0);

		const int CASCADE_COUNT = 3;
		float nearPlane = 0.1f;
		float farPlane = 500.0f;

		vector<float> cascadeEnds = { nearPlane, 30.0f, 100.0f, farPlane };

		float aspect = static_cast<float>(this->contextHeight) / static_cast<float>(this->contextWidth);
		float tanHalfHFOV = tanf(radians(this->FIELD_OF_VIEW / 2.0f));
		float tanHalfVFOV = tanf(radians((this->FIELD_OF_VIEW * aspect) / 2.0f));

		mat4 lightViews[CASCADE_COUNT];
		mat4 lightTranslation = lookAt(vec3(), lightDirection, vec3(0, 1, 0));

		for (int i = 0; i < CASCADE_COUNT; i++) {
			float xn = -cascadeEnds[i] * tanHalfHFOV;
			float xf = -cascadeEnds[i + 1] * tanHalfHFOV;
			float yn = -cascadeEnds[i] * tanHalfVFOV;
			float yf = -cascadeEnds[i + 1] * tanHalfVFOV;

			vec4 frustumCorners[8] = {
				// near face
				vec4(xn,   yn, -cascadeEnds[i], 1.0),
				vec4(-xn,  yn, -cascadeEnds[i], 1.0),
				vec4(xn,  -yn, -cascadeEnds[i], 1.0),
				vec4(-xn, -yn, -cascadeEnds[i], 1.0),

				// far face
				vec4(xf,   yf, -cascadeEnds[i + 1], 1.0),
				vec4(-xf,  yf, -cascadeEnds[i + 1], 1.0),
				vec4(xf,  -yf, -cascadeEnds[i + 1], 1.0),
				vec4(-xf, -yf, -cascadeEnds[i + 1], 1.0)
			};

			vec4 frustumCornersL[8];

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();

			for (uint j = 0; j < 8; j++) {
				vec4 vW = inverse(this->view) * frustumCorners[j];
				frustumCornersL[j] = lightTranslation * vW;

				minX = glm::min(minX, frustumCornersL[j].x);
				maxX = glm::max(maxX, frustumCornersL[j].x);
				minY = glm::min(minY, frustumCornersL[j].y);
				maxY = glm::max(maxY, frustumCornersL[j].y);
				minZ = glm::min(minZ, frustumCornersL[j].z);
				maxZ = glm::max(maxZ, frustumCornersL[j].z);
			}

			lightViews[i] = ortho(minX, maxX, minY, maxY, minZ, maxZ);
		}

		vec3 lightPosition = vec3(0.0f, 25.0f, 50.0f);
		//vec3 lightColour = vec3(0.71f, 0.27f, 0.05f);
		vec3 lightColour = vec3(1.0f);

		// calculate projection matrix
		mat4 lightProjection = lightViews[0];

		mat4 lightView = mat4();

		mat4 lightTransform = lightProjection * lightView;

		this->depthShaderProgram->SetUniform("projection", lightProjection);
		this->depthShaderProgram->SetUniform("view", lightView);

		this->shaderProgram->SetUniform("cascadeEnds", cascadeEnds);

		if (this->viewFromLight)
		{
			this->shaderProgram->SetUniform("projection", lightProjection);
			this->shaderProgram->SetUniform("view", lightView);
		}
		else
		{
			this->shaderProgram->SetUniform("projection", this->projection);
			this->shaderProgram->SetUniform("view", this->view);
		}
		this->shaderProgram->SetUniform("lightPosition", lightPosition);
		this->shaderProgram->SetUniform("lightColour", lightColour);
		this->shaderProgram->SetUniform("lightTransform", lightTransform);

		// 1. first render to depth map
		this->depthMapBuffer->Bind();
		glViewport(0, 0, this->SHADOW_MAP_WIDTH, this->SHADOW_MAP_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);
		this->DrawModels(scene->models, this->depthShaderProgram);

		// 2. then render scene as normal with shadow mapping (using depth map)
		FrameBuffer::Unbind();
		glViewport(0, 0, this->contextWidth, this->contextHeight);
		glClearColor(0.71f, 0.27f, 0.05f, 0);
		//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		texture->Bind();
		this->shaderProgram->SetUniform("textureSampler", 0);

		glActiveTexture(GL_TEXTURE1);
		depthMapTexture->Bind();
		this->shaderProgram->SetUniform("shadowMap", 1);

		this->DrawModels(scene->models, this->shaderProgram);
	}

	void OpenGl::SetViewFromLight(bool viewFromLight)
	{
		this->viewFromLight = viewFromLight;
	}

	void OpenGl::DrawModels(vector<PositionedModel*> models, ShaderProgram* shaderProgram)
	{
		for (PositionedModel* positionedModel : models)
		{
			if (glm::length((*positionedModel->GetPosition() - this->cameraPosition)) > 1000.0f) continue;

			mat4 modelMatrix;
			modelMatrix = translate(modelMatrix, *positionedModel->GetPosition());
			modelMatrix *= static_cast<mat4>(*positionedModel->GetOrientation());
			shaderProgram->SetUniform("model", modelMatrix);

			positionedModel->GetModel()->Draw(shaderProgram);
		}
	}

	void OpenGl::SetViewport(int width, int height)
	{
		this->contextWidth = width;
		this->contextHeight = height;

		// calculate the projection matrix
		this->projection = perspective(this->FIELD_OF_VIEW, static_cast<float>(this->contextWidth) / static_cast<float>(this->contextHeight), 0.1f, 500.0f);
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