#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include <GL\glew.h>

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
	private:
		int id;
	};
}

