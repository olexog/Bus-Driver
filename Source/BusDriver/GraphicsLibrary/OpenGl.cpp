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
		this->pointShaderProgram = new ShaderProgram("Shaders\\PointVertexShader.glsl", "Shaders\\PointFragmentShader.glsl");
		this->segmentShaderProgram = new ShaderProgram("Shaders\\SegmentVertexShader.glsl", "Shaders\\SegmentFragmentShader.glsl");

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

		this->lightSegment = new Segment(vec3(0.0f, 1.0f, 0.0f));
		this->cameraPoint = new Point(10.0f, vec3(0.0f, 0.0f, 1.0f));
		this->frustumEdgeSegment = new Segment(vec3(0.0f, 0.0f, 1.0f));
		this->frustumBoundingBoxSegment = new Segment(vec3(1.0f, 0.0f, 1.0f));
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
		// define light parameters
		vec3 lightDirection = normalize(vec3(0.0f, -1.0f, -2.0f));
		vec3 lightPosition = -100.0f * lightDirection;
		//vec3 lightColour = vec3(0.71f, 0.27f, 0.05f);
		vec3 lightColour = vec3(1.0f);

		// calculate view matrices
		this->viewStatic = lookAt(this->cameraPositionStatic, this->cameraPositionStatic + this->cameraDirectionStatic, vec3(0.0f, 1.0f, 0.0f));
		this->viewDynamic = lookAt(this->cameraPositionDynamic, this->cameraPositionDynamic + this->cameraDirectionDynamic, vec3(0.0f, 1.0f, 0.0f));
		mat4 lightView = lookAt(vec3(0.0f), lightDirection, vec3(0.0f, 1.0f, 0.0f));

		// calculate frustum corners in view space
		float tanHalfFOVy = tan(FIELD_OF_VIEW_Y / 2.0f);
		float tanHalfFOVx = this->aspectRatio * tanHalfFOVy;
		float xNear = tanHalfFOVx * Z_NEAR;
		float xFar = tanHalfFOVx * Z_FAR_DYNAMIC;
		float yNear = tanHalfFOVy * Z_NEAR;
		float yFar = tanHalfFOVy * Z_FAR_DYNAMIC;
		vector<vec3> frustumCornersViewSpace = vector<vec3>({
			vec3(xNear, yNear, -Z_NEAR),
			vec3(xNear, -yNear, -Z_NEAR),
			vec3(-xNear, yNear, -Z_NEAR),
			vec3(-xNear, -yNear, -Z_NEAR),
			vec3(xFar, yFar, -Z_FAR_DYNAMIC),
			vec3(xFar, -yFar, -Z_FAR_DYNAMIC),
			vec3(-xFar, yFar, -Z_FAR_DYNAMIC),
			vec3(-xFar, -yFar, -Z_FAR_DYNAMIC)
		});

		// calculate frustum corners in world space
		vector<vec3> frustumCornersWorldSpace = vector<vec3>();
		for (vec3 frustumCornerViewSpace : frustumCornersViewSpace)
		{
			frustumCornersWorldSpace.push_back(Utility::Transform(frustumCornerViewSpace, inverse(this->viewDynamic)));
		}

		// calculate frustum bounding box in light view space
		float left = numeric_limits<float>::max();
		float right = numeric_limits<float>::lowest();
		float bottom = numeric_limits<float>::max();
		float top = numeric_limits<float>::lowest();
		float zNear = numeric_limits<float>::lowest();
		float zFar = numeric_limits<float>::max();
		for (vec3 frustumCornerWorldSpace : frustumCornersWorldSpace)
		{
			vec3 frustumCornerLightViewSpace = Utility::Transform(frustumCornerWorldSpace, lightView);

			left = glm::min(left, frustumCornerLightViewSpace.x);
			right = glm::max(right, frustumCornerLightViewSpace.x);
			bottom = glm::min(bottom, frustumCornerLightViewSpace.y);
			top = glm::max(top, frustumCornerLightViewSpace.y);
			zNear = glm::max(zNear, frustumCornerLightViewSpace.z);
			zFar = glm::min(zFar, frustumCornerLightViewSpace.z);
		}

		// save frustum bounding box corners in light view space to render them later
		vector<vec3> frustumBoundingBoxCornersLightViewSpace = vector<vec3>({
			vec3(left, bottom, zNear),
			vec3(left, bottom, zFar),
			vec3(left, top, zNear),
			vec3(left, top, zFar),
			vec3(right, bottom, zNear),
			vec3(right, bottom, zFar),
			vec3(right, top, zNear),
			vec3(right, top, zFar)
		});

		// calculate frustum bounding box corners in world space
		vector<vec3> frustumBoundingBoxCornersWorldSpace = vector<vec3>();
		for (vec3 frustumBoundingBoxCornerLightViewSpace : frustumBoundingBoxCornersLightViewSpace)
		{
			frustumBoundingBoxCornersWorldSpace.push_back(Utility::Transform(frustumBoundingBoxCornerLightViewSpace, inverse(lightView)));
		}

		// calculate light projection and transform matrices
		mat4 lightProjection = ortho(left, right, bottom, top, -zNear, -zFar);
		mat4 lightTransform = lightProjection * lightView;

		// set shader uniforms
		this->depthShaderProgram->SetUniform("projection", lightProjection);
		this->depthShaderProgram->SetUniform("view", lightView);

		if (this->viewFromLight)
		{
			this->shaderProgram->SetUniform("projection", lightProjection);
			this->shaderProgram->SetUniform("view", lightView);
		}
		else
		{
			if (this->staticCamera)
			{
				this->shaderProgram->SetUniform("projection", this->projectionStatic);
				this->shaderProgram->SetUniform("view", this->viewStatic);
			}
			else
			{
				this->shaderProgram->SetUniform("projection", this->projectionDynamic);
				this->shaderProgram->SetUniform("view", this->viewDynamic);
			}
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

		// render primitives to visualize frustum, camera, and light positions

		if (!this->staticCamera && !this->viewFromLight)
		{
			return;
		}

		if (this->viewFromLight)
		{
			this->pointShaderProgram->SetUniform("view", lightView);
			this->pointShaderProgram->SetUniform("projection", lightProjection);

			this->segmentShaderProgram->SetUniform("view", lightView);
			this->segmentShaderProgram->SetUniform("projection", lightProjection);
		}
		else
		{
			this->pointShaderProgram->SetUniform("view", viewStatic);
			this->pointShaderProgram->SetUniform("projection", projectionStatic);

			this->segmentShaderProgram->SetUniform("view", viewStatic);
			this->segmentShaderProgram->SetUniform("projection", projectionStatic);
		}

		// draw camera as a point
		this->DrawPoint(this->cameraPoint, this->cameraPositionDynamic);

		// draw frustum edges
		this->DrawCube(this->frustumEdgeSegment, frustumCornersWorldSpace);

		// draw frustum bounding box
		this->DrawCube(this->frustumBoundingBoxSegment, frustumBoundingBoxCornersWorldSpace);

		// draw light as a segment
		this->DrawSegment(this->lightSegment, lightPosition, vec3(0.0f));
	}

	void OpenGl::DrawPoint(Point* point, vec3 position)
	{
		this->pointShaderProgram->SetUniform("position", position);
		point->Draw(this->pointShaderProgram);
	}

	void OpenGl::DrawSegment(Segment* segment, vec3 startPoint, vec3 endPoint)
	{
		this->segmentShaderProgram->SetUniform("startPoint", startPoint);
		this->segmentShaderProgram->SetUniform("endPoint", endPoint);
		segment->Draw(this->segmentShaderProgram);
	}

	void OpenGl::DrawCube(Segment* segment, vector<vec3> corners)
	{
		DrawSegment(segment, corners[0], corners[1]);
		DrawSegment(segment, corners[1], corners[3]);
		DrawSegment(segment, corners[3], corners[2]);
		DrawSegment(segment, corners[2], corners[0]);

		DrawSegment(segment, corners[0], corners[4]);
		DrawSegment(segment, corners[1], corners[5]);
		DrawSegment(segment, corners[2], corners[6]);
		DrawSegment(segment, corners[3], corners[7]);

		DrawSegment(segment, corners[4], corners[5]);
		DrawSegment(segment, corners[5], corners[7]);
		DrawSegment(segment, corners[7], corners[6]);
		DrawSegment(segment, corners[6], corners[4]);
	}

	void OpenGl::SetViewFromLight(bool viewFromLight)
	{
		this->viewFromLight = viewFromLight;
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

	void OpenGl::SetViewport(int width, int height)
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