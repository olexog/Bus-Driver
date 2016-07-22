#include "PositionedModel.h"

namespace GraphicsLibrary
{
	PositionedModel::PositionedModel()
	{

	}

	PositionedModel::~PositionedModel()
	{
		if (this->model != nullptr)
		{
			delete this->model;
		}
	}
}
