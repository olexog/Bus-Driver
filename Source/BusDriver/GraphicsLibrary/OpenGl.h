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
#include "GraphicsUtility.h"
#include "Font.h"
#include "Point.h"
#include "Segment.h"
#include "Alignment.h"
#include "RenderBuffer.h"
#include "TextDrawing.h"

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
		void DrawPoint(vec3 position, float size, vec3 colour);

		///<summary>Draws a line segment between the specified points.</summary>
		void DrawSegment(vec3 startPoint, vec3 endPoint, vec3 colour);

		///<summary>Draws a cube connecting the specified 8 points.</summary>
		void DrawCube(vector<vec3> corners, vec3 colour);

		///<summary>Sets the rendering context's size.</summary>
		void SetContextSize(int width, int height);

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

		///<summary>Sets whether the shadow map should be rendered instead of the normal world view.</summary>
		void SetRenderShadowMap(bool renderShadowMap);

		///<summary>Sets the active cascade to visualize for debug reasons.</summary>
		void SetCascadeToVisualize(int cascadeIndex);

		void AddTextDrawing(TextDrawing* textDrawing);
	private:
		///<summary>Initializes the screen FBO according to the current context size.</summary>
		void InitializeScreenFrameBuffer();

		void DrawModels(vector<PositionedModel*> models, ShaderProgram* shaderProgram);

		///<summary>Draws the specified text with the given scale, aligment, margin and colour.</summary>
		void DrawText(string text, float scale, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, vec2 margin, vec3 colour);

		ShaderProgram* shaderProgram;
		ShaderProgram* depthShaderProgram;
		ShaderProgram* fontShaderProgram;
		ShaderProgram* pointShaderProgram;
		ShaderProgram* segmentShaderProgram;
		ShaderProgram* debugShaderProgram;
		ShaderProgram* screenShaderProgram;

		const int SHADOW_MAP_WIDTH = 1024;
		const int SHADOW_MAP_HEIGHT = 1024;

		const int CASCADE_COUNT = 3;

		const float FIELD_OF_VIEW_Y = 45.0f;
		const float Z_NEAR = 0.1f;
		const float Z_FAR_STATIC = 500.0f;
		const float Z_FAR_DYNAMIC = 250.0f;

		int contextWidth;
		int contextHeight;

		float aspectRatio;

		vector<FrameBuffer*> depthMapBuffers;
		vector<Texture*> depthMapTextures;

		FrameBuffer* screenFrameBuffer;
		Texture* screenTexture;
		RenderBuffer* screenRenderBuffer;

		vector<float> cascadeZEnds;

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
		bool renderShadowMap = false;

		int cascadeToVisualize;

		VertexBuffer* quadVertices;
		VertexBuffer* quadTexCoords;
		GLuint quadVertexArrayId;

		Font* arial;

		Point* point;
		Segment* segment;

		vector<TextDrawing*> textDrawings;
	};
}