#include "Map.h"

Map::Map(vector<Thingy*> thingies, vector<PositionedThingy*> positionedThingies)
{
	this->thingies = thingies;
	this->positionedThingies = positionedThingies;
}

Map::~Map()
{
	for (Thingy* thingy : this->thingies)
	{
		delete thingy;
	}

	for (PositionedThingy* positionedThingy : this->positionedThingies)
	{
		delete positionedThingy;
	}
}

Scene* Map::CreateScene()
{
	Scene* result = new Scene();

	for (PositionedThingy* positionedThingy : this->positionedThingies)
	{
		result->models.push_back(new PositionedModel(positionedThingy->GetThingy()->GetModel(), positionedThingy->GetPosition(), positionedThingy->GetOrientation()));
	}

	return result;
}

Playground* Map::CreatePlayground(Physics* physics)
{
	Playground* result = new Playground(physics);

	for (PositionedThingy* positionedThingy : this->positionedThingies)
	{
		result->AddActor(new Actor(physics, positionedThingy->GetThingy()->GetBody(), positionedThingy->GetPosition(), positionedThingy->GetOrientation()));
	}

	return result;
}
