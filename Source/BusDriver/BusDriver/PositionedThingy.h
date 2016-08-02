#pragma once

#include <glm\vec3.hpp>
#include <glm\gtx\quaternion.hpp>

#include "Thingy.h"

class PositionedThingy
{
public:
	PositionedThingy(Thingy* thingy, vec3* position, quat* orientation);
	~PositionedThingy();

	///<summary>Gets the reference to the positioned thingy instance.</summary>
	Thingy* GetThingy();

	///<summary>Gets the reference to the thingy's position.</summary>
	vec3* GetPosition();

	///<summary>Gets the reference to the thingy's orientation.</summary>
	quat* GetOrientation();
private:
	Thingy* thingy;

	vec3* position;
	quat* orientation;
};

