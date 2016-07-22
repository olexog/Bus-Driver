#include "Scene.h"


namespace GraphicsLibrary
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		set<Model*> modelAdresses;
		for (PositionedModel* positionedModel : this->models)
		{
			modelAdresses.insert(positionedModel->model);
			delete positionedModel;
		}

		for (Model* modelAdress : modelAdresses)
		{
			delete modelAdress;
		}
	}
}
