#include "Scene.h"


namespace GraphicsLibrary
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		for (PositionedModel* positionedModel : this->models)
		{
			delete positionedModel;
		}
	}
}
