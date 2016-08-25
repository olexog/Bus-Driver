#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include <GL\glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	class ShaderProgram
	{
	public:
		ShaderProgram() : id(0) {}
		ShaderProgram(string vertexShaderFileName, string fragmentShaderFileName);
		~ShaderProgram();

		void Use();

		static void Disable();

		void SetUniform(string name, vec3 matrix);
		void SetUniform(string name, mat4 matrix);
		void SetUniform(string name, int value);
	private:
		GLuint id;
	};
}