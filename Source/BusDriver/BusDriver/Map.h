#pragma once

#include <vector>

#include "Thingy.h"
#include "PositionedThingy.h"
#include "Scene.h"
#include "Physics.h"
#include "Playground.h"

using namespace std;
using namespace PhysicsLibrary;

class Map
{
public:
	Map(vector<Thingy*> thingies, vector<PositionedThingy*> positionedThingies);
	~Map();

	void AddThingy(Thingy* thingy);
	void AddPositionedThingy(PositionedThingy* positionedThingy);

	Scene* CreateScene();
	Playground* CreatePlayground(Physics* physics);
private:
	vector<Thingy*> thingies;

	vector<PositionedThingy*> positionedThingies;
};

