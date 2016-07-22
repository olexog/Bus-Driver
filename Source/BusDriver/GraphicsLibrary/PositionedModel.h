#pragma once

#include "Model.h"

#include <glm\vec3.hpp>
#include <glm\vec3.hpp>

using namespace glm;

namespace GraphicsLibrary
{
	class PositionedModel
	{
	public:
		PositionedModel();
		PositionedModel(Model* model, vec3 position, vec3 rotation) : model(model), position(position), rotation(rotation) {}
		~PositionedModel();

		Model* model;
		vec3 position;
		vec3 rotation;
	};
}

