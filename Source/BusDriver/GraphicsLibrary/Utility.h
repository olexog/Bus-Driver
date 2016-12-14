#pragma once

#include <string>
#include <vector>
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <SOIL\SOIL.h>

#include <glm/glm.hpp>

#include "Texture.h"

using namespace std;

using namespace glm;

namespace GraphicsLibrary
{
	class Utility
	{
	public:
		///<summary>Represents the whitespace characters.</summary>
		static constexpr const char* WHITESPACES = " \t\r\n";

		static string GetDirectory(string fileName);

		static string TrimStart(string s);

		static string TrimEnd(string s);

		static string Trim(string s);

		static vector<string> Split(string s, char delimiter);

		static vector<string> Split(string s, string delimiter);

		static Texture* LoadTexture(string fileName);

		static vec3 Transform(vec3 vector, mat4 matrix);
	};
}