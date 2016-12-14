#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Scene.h"
#include "ShaderProgram.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "Utility.h"
#include "Point.h"
#include "Segment.h"

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	///<summary>Represents an OpenGL renderer to draw the scene to the current context.</summary>
	class OpenGl
	{
	public:
		///<summary>Initializes a new OpenGL instance with the context's size.</summary>
		OpenGl(int width, int height);

		///<summmary>Deinitializes the OpenGL instance.</summary>
		~OpenGl();

		///<summary>Draws to the context.</summary>
		void Draw(Scene* scene);

		///<summary>Draws a point to the specified position.</summary>
		void DrawPoint(Point* point, vec3 position);

		///<summary>Draws a line segment between the specified points.</summary>
		void DrawSegment(Segment* segment, vec3 startPoint, vec3 endPoint);

		///<summary>Draws a cube connecting the specified 8 points.</summary>
		void DrawCube(Segment* segment, vector<vec3> corners);

		///<summary>Sets the rendering context's size.</summary>
		void SetViewport(int width, int height);

		///<summary>Sets the camera mode to static or dynamic.</summary>
		void SetCameraMode(bool isStatic);

		///<summary>Sets the camera pose.</summary>
		void SetCameraStatic(vec3 position, vec3 direction);

		///<summary>Sets the camera pose.</summary>
		void SetCameraDynamic(vec3 position, vec3 direction);

		///<summary>Sets whether the scene should be rendered in wireframe mode or not.</summary>
		void SetWireframeMode(bool wireframeMode);

		///<summary>Sets whether the scene should be rendered from the light's position and with the light's view transformation.</summary>
		void SetViewFromLight(bool viewFromLight);
	private:
		void DrawModels(vector<PositionedModel*> models, ShaderProgram* shaderProgram);

		ShaderProgram* shaderProgram;
		ShaderProgram* depthShaderProgram;
		ShaderProgram* pointShaderProgram;
		ShaderProgram* segmentShaderProgram;

		const int SHADOW_MAP_WIDTH = 1024;
		const int SHADOW_MAP_HEIGHT = 1024;

		const float FIELD_OF_VIEW_Y = 45.0f;
		const float Z_NEAR = 0.1f;
		const float Z_FAR_STATIC = 500.0f;
		const float Z_FAR_DYNAMIC = 50.0f;

		int contextWidth;
		int contextHeight;

		float aspectRatio;

		FrameBuffer* depthMapBuffer;
		Texture* depthMapTexture;

		Texture* texture;

		bool staticCamera;

		mat4 projectionStatic;
		mat4 projectionDynamic;
		mat4 viewStatic;
		mat4 viewDynamic;

		vec3 cameraPositionStatic;
		vec3 cameraDirectionStatic;
		vec3 cameraPositionDynamic;
		vec3 cameraDirectionDynamic;

		bool wireframeMode = false;
		bool viewFromLight = false;

		Segment* lightSegment;
		Point* cameraPoint;
		Segment* frustumEdgeSegment;
		Segment* frustumBoundingBoxSegment;
	};
}