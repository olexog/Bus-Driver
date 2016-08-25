#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

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

		///<summary>Sets the rendering context's size.</summary>
		void SetViewport(int width, int height);

		///<summary>Sets the camera pose.</summary>
		void SetCamera(vec3 position, vec3 direction);

		///<summary>Sets whether the scene should be rendered in wireframe mode or not.</summary>
		void SetWireframeMode(bool wireframeMode);
	private:
		ShaderProgram* shaderProgram;
		ShaderProgram* colouredShaderProgram;

		mat4 projection;
		mat4 view;

		vec3 cameraPosition;
		vec3 cameraDirection;

		bool wireframeMode = false;
	};
}