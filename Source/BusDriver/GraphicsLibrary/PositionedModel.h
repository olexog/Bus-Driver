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
		PositionedModel(Model* model, vec3* position, quat* orientation) : model(model), position(position), orientation(orientation) {}
		~PositionedModel();

		Model* GetModel();
		vec3* GetPosition();
		quat* GetOrientation();

	private:
		Model* model;
		vec3* position;
		quat* orientation;
	};
}

