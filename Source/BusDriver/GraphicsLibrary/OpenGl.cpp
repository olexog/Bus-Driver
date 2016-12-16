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
		this->SetContextSize(width, height);

		// load shader programs
		this->shaderProgram = new ShaderProgram("Shaders\\VertexShader.glsl", "Shaders\\FragmentShader.glsl");
		this->depthShaderProgram = new ShaderProgram("Shaders\\DepthVertexShader.glsl", "Shaders\\DepthFragmentShader.glsl");
		this->fontShaderProgram = new ShaderProgram("Shaders\\FontVertexShader.glsl", "Shaders\\FontFragmentShader.glsl");
		this->pointShaderProgram = new ShaderProgram("Shaders\\PointVertexShader.glsl", "Shaders\\PointFragmentShader.glsl");
		this->segmentShaderProgram = new ShaderProgram("Shaders\\SegmentVertexShader.glsl", "Shaders\\SegmentFragmentShader.glsl");

		// enable the depth buffer for depth testing
		glEnable(GL_DEPTH_TEST);

		// initialize depth maps
		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			this->depthMapBuffers.push_back(new FrameBuffer());
			this->depthMapTextures.push_back(new Texture());
			this->depthMapTextures[i]->Bind();
			this->depthMapTextures[i]->LoadData(this->SHADOW_MAP_WIDTH, this->SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			// clamping to the border needed to ignore the areas which is not covered by the shadow map
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			GLfloat borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			this->depthMapBuffers[i]->Bind();
			this->depthMapTextures[i]->AttachToFramebuffer(GL_DEPTH_ATTACHMENT);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			FrameBuffer::Unbind();
		}

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

		// initialize cascade z-ends
		float lambda = 0.5f;
		cascadeZEnds = vector<float>();
		for (int i = 0; i <= CASCADE_COUNT; i++)
		{
			float logarithmicZ = Z_NEAR * pow(Z_FAR_DYNAMIC / Z_NEAR, static_cast<float>(i) / static_cast<float>(CASCADE_COUNT));
			float uniformZ = Z_NEAR + (Z_FAR_DYNAMIC - Z_NEAR) * (static_cast<float>(i) / static_cast<float>(CASCADE_COUNT));
			float mixedZ = lambda * logarithmicZ + (1.0f - lambda) * uniformZ;
			cascadeZEnds.push_back(mixedZ);
		}

		this->shaderProgram->SetUniform("cascadeEnds", cascadeZEnds);

		// initialize fonts
		this->arial = new Font("Fonts\\ARIAL.TTF", 50, 0, 255);

		// initialize primitives
		this->point = new Point();
		this->segment = new Segment();
	}

	OpenGl::~OpenGl()
	{
		delete this->shaderProgram;
		delete this->depthShaderProgram;

		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			delete this->depthMapTextures[i];
			delete this->depthMapBuffers[i];
		}
	}

	void OpenGl::Draw(Scene* scene)
	{
		// define light parameters
		vec3 lightDirection = normalize(vec3(0.0f, -1.0f, -2.0f));
		vec3 lightPosition = -100.0f * lightDirection;
		//vec3 lightColour = vec3(0.71f, 0.27f, 0.05f);
		vec3 lightColour = vec3(1.0f);

		// calculate view matrices
		this->viewStatic = lookAt(this->cameraPositionStatic, this->cameraPositionStatic + this->cameraDirectionStatic, vec3(0.0f, 1.0f, 0.0f));
		this->viewDynamic = lookAt(this->cameraPositionDynamic, this->cameraPositionDynamic + this->cameraDirectionDynamic, vec3(0.0f, 1.0f, 0.0f));
		mat4 inverseView = inverse(this->viewDynamic);
		mat4 lightView = lookAt(vec3(0.0f), lightDirection, vec3(0.0f, 1.0f, 0.0f));
		mat4 inverseLightView = inverse(lightView);

		// CSM
		float tanHalfFOVy = tan(FIELD_OF_VIEW_Y / 2.0f);
		float tanHalfFOVx = this->aspectRatio * tanHalfFOVy;
		vector<vector<vec3>> frustumCornersWorldSpace = vector<vector<vec3>>(CASCADE_COUNT);
		vector<vector<vec3>> frustumBoundingBoxCornersWorldSpace = vector<vector<vec3>>(CASCADE_COUNT);
		vector<mat4> lightProjections = vector<mat4>(CASCADE_COUNT);

		for (int cascadeIndex = 0; cascadeIndex < CASCADE_COUNT; cascadeIndex++)
		{
			float zNear = this->cascadeZEnds[cascadeIndex];
			float zFar = this->cascadeZEnds[cascadeIndex + 1];

			// calculate frustum corners in view space
			float xNear = tanHalfFOVx * zNear;
			float xFar = tanHalfFOVx * zFar;
			float yNear = tanHalfFOVy * zNear;
			float yFar = tanHalfFOVy * zFar;
			vector<vec3> frustumCornersViewSpace = vector<vec3>({
				vec3(xNear, yNear, -zNear),
				vec3(xNear, -yNear, -zNear),
				vec3(-xNear, yNear, -zNear),
				vec3(-xNear, -yNear, -zNear),
				vec3(xFar, yFar, -zFar),
				vec3(xFar, -yFar, -zFar),
				vec3(-xFar, yFar, -zFar),
				vec3(-xFar, -yFar, -zFar)
			});

			// calculate frustum corners in world space
			frustumCornersWorldSpace[cascadeIndex] = vector<vec3>();
			for (vec3 frustumCornerViewSpace : frustumCornersViewSpace)
			{
				frustumCornersWorldSpace[cascadeIndex].push_back(Utility::Transform(frustumCornerViewSpace, inverseView));
			}

			// calculate frustum bounding box in light view space
			float left = numeric_limits<float>::max();
			float right = numeric_limits<float>::lowest();
			float bottom = numeric_limits<float>::max();
			float top = numeric_limits<float>::lowest();
			float near = numeric_limits<float>::lowest();
			float far = numeric_limits<float>::max();
			for (vec3 frustumCornerWorldSpace : frustumCornersWorldSpace[cascadeIndex])
			{
				vec3 frustumCornerLightViewSpace = Utility::Transform(frustumCornerWorldSpace, lightView);

				left = glm::min(left, frustumCornerLightViewSpace.x);
				right = glm::max(right, frustumCornerLightViewSpace.x);
				bottom = glm::min(bottom, frustumCornerLightViewSpace.y);
				top = glm::max(top, frustumCornerLightViewSpace.y);
				near = glm::max(near, frustumCornerLightViewSpace.z);
				far = glm::min(far, frustumCornerLightViewSpace.z);
			}

			// save frustum bounding box corners in light view space to render them later
			vector<vec3> frustumBoundingBoxCornersLightViewSpace = vector<vec3>({
				vec3(left, bottom, near),
				vec3(left, bottom, far),
				vec3(left, top, near),
				vec3(left, top, far),
				vec3(right, bottom, near),
				vec3(right, bottom, far),
				vec3(right, top, near),
				vec3(right, top, far)
			});

			// calculate frustum bounding box corners in world space
			frustumBoundingBoxCornersWorldSpace[cascadeIndex] = vector<vec3>();
			for (vec3 frustumBoundingBoxCornerLightViewSpace : frustumBoundingBoxCornersLightViewSpace)
			{
				frustumBoundingBoxCornersWorldSpace[cascadeIndex].push_back(Utility::Transform(frustumBoundingBoxCornerLightViewSpace, inverseLightView));
			}

			// calculate light projection matrix
			lightProjections[cascadeIndex] = ortho(left, right, bottom, top, -near, -far);

			// set depth shader uniforms
			this->depthShaderProgram->SetUniform("projection", lightProjections[cascadeIndex]);
			this->depthShaderProgram->SetUniform("view", lightView);

			// 1. first render to depth map
			this->depthMapBuffers[cascadeIndex]->Bind();
			glViewport(0, 0, this->SHADOW_MAP_WIDTH, this->SHADOW_MAP_HEIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);
			this->DrawModels(scene->models, this->depthShaderProgram);
		}

		// set main shader uniforms
		if (this->viewFromLight)
		{
			this->shaderProgram->SetUniform("shadowsOn", 0.0f);

			this->shaderProgram->SetUniform("projection", lightProjections[this->cascadeToVisualize]);
			this->shaderProgram->SetUniform("view", lightView);
		}
		else
		{
			if (this->staticCamera)
			{
				this->shaderProgram->SetUniform("shadowsOn", 0.0f);

				this->shaderProgram->SetUniform("projection", this->projectionStatic);
				this->shaderProgram->SetUniform("view", this->viewStatic);
			}
			else
			{
				this->shaderProgram->SetUniform("shadowsOn", 1.0f);

				this->shaderProgram->SetUniform("projection", this->projectionDynamic);
				this->shaderProgram->SetUniform("view", this->viewDynamic);
			}
		}
		this->shaderProgram->SetUniform("lightPosition", lightPosition);
		this->shaderProgram->SetUniform("lightColour", lightColour);

		// 2. then render scene as normal with shadow mapping (using depth maps)
		FrameBuffer::Unbind();
		glViewport(0, 0, this->contextWidth, this->contextHeight);
		glClearColor(0.71f, 0.27f, 0.05f, 0);
		//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		texture->Bind();
		this->shaderProgram->SetUniform("textureSampler", 0);

		//this->shaderProgram->SetUniform("cascadeNumber", 1);

		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			glActiveTexture(GL_TEXTURE1 + i);
			depthMapTextures[i]->Bind();
			this->shaderProgram->SetUniform("shadowMaps[" + to_string(i) + "]", 1 + i);

			this->shaderProgram->SetUniform("lightTransforms[" + to_string(i) + "]", lightProjections[i] * lightView);
		}

		this->DrawModels(scene->models, this->shaderProgram);

		// draw text

		this->fontShaderProgram->SetUniform("projection", ortho(0.0f, static_cast<float>(this->contextWidth), 0.0f, static_cast<float>(this->contextHeight)));
		this->arial->DrawText(this->fontShaderProgram, "Epic win!!!", vec2(10.0f, 10.0f), 1.0f, vec3(1.0f, 0.0f, 0.0f));
		this->arial->DrawText(this->fontShaderProgram, "Active cascade index: " + to_string(this->cascadeToVisualize),
			vec2(20.0f, this->contextHeight - 40.0f), 0.5f, vec3(0.0f, 0.0f, 1.0f));

		// render primitives to visualize frustum, camera, and light positions

		if (!this->staticCamera && !this->viewFromLight)
		{
			return;
		}

		if (this->viewFromLight)
		{
			this->pointShaderProgram->SetUniform("view", lightView);
			this->pointShaderProgram->SetUniform("projection", lightProjections[this->cascadeToVisualize]);

			this->segmentShaderProgram->SetUniform("view", lightView);
			this->segmentShaderProgram->SetUniform("projection", lightProjections[cascadeToVisualize]);
		}
		else
		{
			this->pointShaderProgram->SetUniform("view", viewStatic);
			this->pointShaderProgram->SetUniform("projection", projectionStatic);

			this->segmentShaderProgram->SetUniform("view", viewStatic);
			this->segmentShaderProgram->SetUniform("projection", projectionStatic);
		}

		// draw camera as a point
		this->DrawPoint(this->cameraPositionDynamic, 10.0f, vec3(0.0f, 0.0f, 1.0f));

		// draw frustum edges
		this->DrawCube(frustumCornersWorldSpace[cascadeToVisualize], vec3(0.0f, 0.0f, 1.0f));

		// draw frustum bounding box
		this->DrawCube(frustumBoundingBoxCornersWorldSpace[cascadeToVisualize], vec3(1.0f, 0.0f, 1.0f));

		// draw light as a segment
		this->DrawSegment(lightPosition, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	}

	void OpenGl::DrawPoint(vec3 position, float size, vec3 colour)
	{
		this->point->Draw(this->pointShaderProgram, position, size, colour);
	}

	void OpenGl::DrawSegment(vec3 startPoint, vec3 endPoint, vec3 colour)
	{
		this->segment->Draw(this->segmentShaderProgram, startPoint, endPoint, colour);
	}

	void OpenGl::DrawCube(vector<vec3> corners, vec3 colour)
	{
		this->DrawSegment(corners[0], corners[1], colour);
		this->DrawSegment(corners[1], corners[3], colour);
		this->DrawSegment(corners[3], corners[2], colour);
		this->DrawSegment(corners[2], corners[0], colour);

		this->DrawSegment(corners[0], corners[4], colour);
		this->DrawSegment(corners[1], corners[5], colour);
		this->DrawSegment(corners[2], corners[6], colour);
		this->DrawSegment(corners[3], corners[7], colour);

		this->DrawSegment(corners[4], corners[5], colour);
		this->DrawSegment(corners[5], corners[7], colour);
		this->DrawSegment(corners[7], corners[6], colour);
		this->DrawSegment(corners[6], corners[4], colour);
	}

	void OpenGl::SetViewFromLight(bool viewFromLight)
	{
		this->viewFromLight = viewFromLight;
	}

	void OpenGl::SetCascadeToVisualize(int cascadeIndex)
	{
		this->cascadeToVisualize = cascadeIndex;
	}

	void OpenGl::DrawModels(vector<PositionedModel*> models, ShaderProgram* shaderProgram)
	{
		vec3 cameraPosition = this->staticCamera ? this->cameraPositionStatic : this->cameraPositionDynamic;

		for (PositionedModel* positionedModel : models)
		{
			if (glm::length((*positionedModel->GetPosition() - cameraPosition)) > 1000.0f) continue;

			mat4 modelMatrix;
			modelMatrix = translate(modelMatrix, *positionedModel->GetPosition());
			modelMatrix *= static_cast<mat4>(*positionedModel->GetOrientation());
			shaderProgram->SetUniform("model", modelMatrix);

			positionedModel->GetModel()->Draw(shaderProgram);
		}
	}

	void OpenGl::SetContextSize(int width, int height)
	{
		this->contextWidth = width;
		this->contextHeight = height;

		this->aspectRatio = static_cast<float>(this->contextWidth) / static_cast<float>(this->contextHeight);

		// calculate the projection matrix
		this->projectionStatic = perspective(FIELD_OF_VIEW_Y, aspectRatio, Z_NEAR, Z_FAR_STATIC);
		this->projectionDynamic = perspective(FIELD_OF_VIEW_Y, aspectRatio, Z_NEAR, Z_FAR_DYNAMIC);
	}

	void OpenGl::SetWireframeMode(bool wireframeMode)
	{
		this->wireframeMode = wireframeMode;
	}

	void OpenGl::SetCameraMode(bool isStatic)
	{
		this->staticCamera = isStatic;
	}

	void OpenGl::SetCameraStatic(vec3 position, vec3 direction)
	{
		this->cameraPositionStatic = position;
		this->cameraDirectionStatic = direction;
	}

	void OpenGl::SetCameraDynamic(vec3 position, vec3 direction)
	{
		this->cameraPositionDynamic = position;
		this->cameraDirectionDynamic = direction;
	}
}