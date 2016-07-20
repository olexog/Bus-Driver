#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include <GL\glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <PxPhysicsAPI.h>

using namespace std;
using namespace physx;

namespace GraphicsLibrary
{
	class ShaderProgram
	{
	public:
		ShaderProgram() : id(0) {}
		ShaderProgram(string vertexShaderFileName, string fragmentShaderFileName);
		~ShaderProgram();

		void Use();

		void SetUniform(string name, glm::mat4 matrix);
	private:
		GLuint id;
	};
}