#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <iostream>

#include <glm\vec2.hpp>
#include <glm\vec3.hpp>

#include "Model.h"
#include "Utility.h"

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	class ModelReader
	{
	public:
		static Model* Read(string fileName);
		static Model* Read(string fileName, vector<vec3> &vertices);
	};
}