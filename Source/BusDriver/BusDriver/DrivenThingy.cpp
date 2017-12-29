#include "DrivenThingy.h"

DrivenThingy::DrivenThingy(vector<vec3> wheelVertices, Physics* physics, float chassisWidth, float chassisHeight, float chassisLength,
	Model* wheelModel, Model* chassisModel, PxVec3 startingPosition, Playground* playground)
{
	for (int i = 0; i < 4; i++)
	{
		Shape* wheel = PhysicsUtility::ShapeFromConvexTriangles(wheelVertices, physics);
		wheel->SetType(Shape::Type::WHEEL);
		wheel->SetPosePointer(wheelLocalPositions[i], wheelLocalOrientations[i]);

		wheels.push_back(wheel);
	}

	this->chassisMesh = createChassisMesh(PxVec3(chassisWidth, chassisHeight, chassisLength), *physics->GetPhysics(), *physics->GetCooking());
	this->chassisGeometry = new PxConvexMeshGeometry(chassisMesh);
	this->chassis = new Shape(physics, chassisGeometry, this->chassisLocalPosition, this->chassisLocalOrientation, Shape::Type::CHASSIS);

	this->wheelModel = wheelModel;
	this->chassisModel = chassisModel;

	this->chassisMass = 1500.0f;
	this->chassisDims = PxVec3(chassisWidth, chassisHeight, chassisLength);
	this->chassisMOI = PxVec3
	(
		(chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f
	);
	this->chassisCMOffset = PxVec3(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	this->wheelMass = 20.0f;
	this->wheelRadius = 0.704f;
	this->wheelWidth = 0.748f;
	this->wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;

	// the wheel base of the Ikarus 260
	this->wheelBase = 5.4f;
	// the difference between the bottom and the wheel's centre
	this->prolapse = 0.7f;

	this->rearAxisOffset = 2.7f;
	this->frontAxisOffset = 2.7f;

	this->vehicle = new Vehicle(physics, chassis, wheels, startingPosition,
		chassisMass, chassisDims, chassisMOI, physics->GetMaterial(), chassisCMOffset,
		wheelMass, wheelRadius, wheelWidth, wheelMOI, physics->GetMaterial(), 4,
		this->frontAxisOffset, this->rearAxisOffset, prolapse);
	playground->AddActor(vehicle);
	vehicle->SetToRestState();
}


DrivenThingy::~DrivenThingy()
{
}

void DrivenThingy::AddToScene(Scene* scene)
{
	for (int i = 0; i < 4; i++)
	{
		scene->models.push_back(new PositionedModel(wheelModel, wheelPositions[i], wheelOrientations[i]));
	}
	scene->models.push_back(new PositionedModel(chassisModel, chassisPosition, chassisOrientation));
}

void DrivenThingy::Update()
{
	// update physics shapes
	for (Shape* wheel : wheels)
	{
		wheel->Update();
	}
	chassis->Update();

	// calculate the global positions of the vehicle's wheels
	for (int i = 0; i < wheelLocalPositions.size(); i++)
	{
		mat4 localTranslation;
		localTranslation = translate(localTranslation, *wheelLocalPositions[i]);
		mat4 localRotation = static_cast<mat4>(*wheelLocalOrientations[i]);
		mat4 globalTranslation;
		globalTranslation = translate(globalTranslation, vehicle->GetPosition());
		mat4 globalRotation = static_cast<mat4>(vehicle->GetRotation());
		mat4 modelMatrix = globalTranslation * globalRotation * localTranslation * localRotation;
		vec3 position = vec3(modelMatrix[3]);
		quat orientation = static_cast<quat>(modelMatrix);

		wheelPositions[i]->x = position.x;
		wheelPositions[i]->y = position.y;
		wheelPositions[i]->z = position.z;

		wheelOrientations[i]->x = orientation.x;
		wheelOrientations[i]->y = orientation.y;
		wheelOrientations[i]->z = orientation.z;
		wheelOrientations[i]->w = orientation.w;
	}
	// calculate the global position of the chassis
	{
		mat4 localTranslation;
		localTranslation = translate(localTranslation, *chassisLocalPosition);
		mat4 localRotation = static_cast<mat4>(*chassisLocalOrientation);
		mat4 globalTranslation;
		globalTranslation = translate(globalTranslation, vehicle->GetPosition());
		mat4 globalRotation = static_cast<mat4>(vehicle->GetRotation());
		mat4 modelMatrix = globalTranslation * globalRotation * localTranslation * localRotation;

		vec3 position = vec3(modelMatrix[3]);
		quat orientation = static_cast<quat>(modelMatrix);

		chassisPosition->x = position.x;
		chassisPosition->y = position.y;
		chassisPosition->z = position.z;

		chassisOrientation->x = orientation.x;
		chassisOrientation->y = orientation.y;
		chassisOrientation->z = orientation.z;
		chassisOrientation->w = orientation.w;
	}
}

Vehicle* DrivenThingy::GetVehicle()
{
	return this->vehicle;
}
