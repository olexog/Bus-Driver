#include "Map.h"

Map::Map(vector<Thingy*> thingies, vector<PositionedThingy> positionedThingies)
{
	this->thingies = thingies;
	this->positionedThingies = positionedThingies;
}

Map::~Map()
{
}

Scene* Map::CreateScene()
{
	Scene* result = new Scene();

	for (PositionedThingy positionedThingy : this->positionedThingies)
	{
		result->models.push_back(new PositionedModel(positionedThingy.GetThingy()->GetModel(), positionedThingy.GetPosition(), positionedThingy.GetOrientation()));
	}

	return result;
}
