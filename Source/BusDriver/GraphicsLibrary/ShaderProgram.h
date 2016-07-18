#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include <GL\glew.h>

using namespace std;

namespace GraphicsLibrary
{
	class ShaderProgram
	{
	public:
		ShaderProgram() : id(0) {}
		ShaderProgram(string vertexShaderFileName, string fragmentShaderFileName);
		~ShaderProgram();

		void Use();
	private:
		int id;
	};
}

