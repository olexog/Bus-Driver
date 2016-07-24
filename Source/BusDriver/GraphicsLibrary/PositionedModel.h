#pragma once

#include "Model.h"

#include <glm\vec3.hpp>
#include <glm\gtc\quaternion.hpp>

using namespace glm;

namespace GraphicsLibrary
{
	class PositionedModel
	{
	public:
		PositionedModel();
		PositionedModel(Model* model, vec3 position, quat rotation) : model(model), position(position), rotation(rotation) {}
		~PositionedModel();

		Model* model;
		vec3 position;
		quat rotation;
	};
}

