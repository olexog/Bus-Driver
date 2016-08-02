#include "Thingy.h"

Thingy::Thingy(Model* model)
{
	this->model = model;
}

Thingy::~Thingy()
{
}

Model* Thingy::GetModel()
{
	return this->model;
}
