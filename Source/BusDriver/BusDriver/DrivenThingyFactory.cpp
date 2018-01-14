#include "DrivenThingyFactory.h"

DrivenThingyFactory::DrivenThingyFactory(Physics* physics, Playground* playground, Scene* scene)
{
	this->physics = physics;
	this->playgound = playgound;
	this->scene = scene;

	this->wheelVertices = vector<vector<vec3>>();
	this->wheelModels = vector<Model*>();
	this->chassisModels = vector<Model*>();
	this->chassisWidths = vector<float>();
	this->chassisHeights = vector<float>();
	this->chassisLengths = vector<float>();
	this->chassisMasses = vector<float>();
	this->wheelMasses = vector<float>();
	this->wheelRadiuses = vector<float>();
	this->wheelWidths = vector<float>();
	this->prolapses = vector<float>();
	this->frontAxisOffsets = vector<float>();
	this->rearAxisOffsets = vector<float>();

	// Ikarus 260
	this->wheelVertices.push_back(vector<vec3>());
	this->wheelModels.push_back(ModelReader::Read("Models\\ikarus_260_wheel.obj", this->wheelVertices[this->wheelVertices.size - 1]));
	this->chassisModels.push_back(ModelReader::Read("Models\\ikarus_260_body.obj"));
	this->chassisWidths.push_back(2.5f);
	this->chassisHeights.push_back(3.4f);
	this->chassisLengths.push_back(11.0f);
	this->chassisMasses.push_back(1500.0f);
	this->wheelMasses.push_back(20.0f);
	this->wheelRadiuses.push_back(0.704f);
	this->wheelWidths.push_back(0.748f);
	this->prolapses.push_back(0.7f);
	this->frontAxisOffsets.push_back(2.7f);
	this->rearAxisOffsets.push_back(2.7f);

	// Trabant
	this->wheelVertices.push_back(vector<vec3>());
	this->wheelModels.push_back(ModelReader::Read("Models\\trabant_wheel.obj", this->wheelVertices[this->wheelVertices.size - 1]));
	this->chassisModels.push_back(ModelReader::Read("Models\\trabant_body.obj"));
	this->chassisWidths.push_back(1.51f);
	this->chassisHeights.push_back(1.437f);
	this->chassisLengths.push_back(3.56f);
	this->chassisMasses.push_back(650.0f);
	this->wheelMasses.push_back(20.0f);
	this->wheelRadiuses.push_back(0.35f);
	this->wheelWidths.push_back(0.116f);
	this->prolapses.push_back(0.432f);
	this->frontAxisOffsets.push_back(1.182f);
	this->rearAxisOffsets.push_back(0.835f);
}

DrivenThingyFactory::~DrivenThingyFactory()
{
	for (Model* model : this->wheelModels)
	{
		delete model;
	}
	for (Model* model : this->chassisModels)
	{
		delete model;
	}
}

DrivenThingy* DrivenThingyFactory::Create(int id, PxVec3 startingPosition)
{
	DrivenThingy* drivenThingy = new DrivenThingy(this->wheelVertices[id], this->physics, this->chassisWidths[id], this->chassisHeights[id],
		this->chassisLengths[id], this->wheelModels[id], this->chassisModels[id], this->chassisMasses[id], this->wheelMasses[id],
		this->wheelRadiuses[id], this->wheelWidths[id], this->prolapses[id], this->frontAxisOffsets[id], this->rearAxisOffsets[id], 
		startingPosition, this->playgound);
	drivenThingy->AddToScene(this->scene);
	return drivenThingy;
}
