#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <glm\vec2.hpp>
#include <glm\vec3.hpp>

#include "Model.h"

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	class ModelReader
	{
	public:
		static Model* Read(string fileName);
	};
}