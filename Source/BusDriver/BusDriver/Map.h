#pragma once

#include <vector>

#include "Thingy.h"
#include "PositionedThingy.h"
#include "Scene.h"

using namespace std;

class Map
{
public:
	Map(vector<Thingy*> thingies, vector<PositionedThingy> positionedThingies);
	~Map();

	Scene* CreateScene();
private:
	vector<Thingy*> thingies;

	vector<PositionedThingy> positionedThingies;
};

