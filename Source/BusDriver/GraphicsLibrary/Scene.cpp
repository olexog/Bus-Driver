#include "Scene.h"


namespace GraphicsLibrary
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		set<Model*> modelAddresses;
		for (PositionedModel* positionedModel : this->models)
		{
			modelAddresses.insert(positionedModel->model);
			delete positionedModel;
		}

		for (Model* modelAddress : modelAddresses)
		{
			delete modelAddress;
		}
	}
}
