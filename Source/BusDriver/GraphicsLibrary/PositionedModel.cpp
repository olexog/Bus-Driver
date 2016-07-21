#include "PositionedModel.h"


namespace GraphicsLibrary
{
	PositionedModel::PositionedModel()
	{
	}

	PositionedModel::~PositionedModel()
	{
		delete this->model;
	}
}
