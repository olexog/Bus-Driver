#include "PositionedModel.h"

namespace GraphicsLibrary
{
	PositionedModel::~PositionedModel()
	{
	}

	Model* PositionedModel::GetModel()
	{
		return this->model;
	}

	vec3* PositionedModel::GetPosition()
	{
		return this->position;
	}

	quat* PositionedModel::GetOrientation()
	{
		return this->orientation;
	}
}
