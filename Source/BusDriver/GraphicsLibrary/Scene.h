#pragma once

#include <vector>

#include <glm\vec3.hpp>

#include "PositionedModel.h"

using namespace std;
using namespace glm;

namespace GraphicsLibrary
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		vector<PositionedModel*> models;
	};
}

