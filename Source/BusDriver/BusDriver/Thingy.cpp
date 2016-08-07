#include "Thingy.h"

Thingy::Thingy(Model* model, Body* body)
{
	this->model = model;
	this->body = body;
}

Thingy::~Thingy()
{
	delete this->model;
	delete this->body;
}

Model* Thingy::GetModel()
{
	return this->model;
}

Body* Thingy::GetBody()
{
	return this->body;
}
