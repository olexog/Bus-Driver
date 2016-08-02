#include "PositionedThingy.h"

PositionedThingy::PositionedThingy(Thingy* thingy, vec3* position, quat* orientation)
{
	this->thingy = thingy;
	this->position = position;
	this->orientation = orientation;
}

PositionedThingy::~PositionedThingy()
{
}

Thingy* PositionedThingy::GetThingy()
{
	return this->thingy;
}

vec3* PositionedThingy::GetPosition()
{
	return this->position;
}

quat* PositionedThingy::GetOrientation()
{
	return this->orientation;
}
