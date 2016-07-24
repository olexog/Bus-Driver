#include <map>
#include <vector>
#include <fstream>

#include <glm\vec3.hpp>

#include "Window.h"
#include "ShaderProgram.h"
//#include "ModelReader.h"
#include "SceneReader.h"
#include "VertexArray.h"
#include "ColouredVertexArray.h"

#include "Physics.h"
#include "Actor.h"
#include "Vehicle.h"

using namespace GraphicsLibrary;
using namespace PhysicsLibrary;
using namespace std;
using namespace glm;

int main()
{
	// creates the physics environment
	Physics* physics = new Physics();

	Actor* box = new Actor(physics, vec3(1.0f, 1.0f, 1.0f), vec3(0, 5, 0));
	Vehicle* bus = new Vehicle(physics);

	// creates the window
	Window window = Window(640, 480, "Bus Driver");

	Scene* scene = SceneReader::Read("Models\\simple.map");

	Model* boxModel = ModelReader::Read("Models\\box.obj");
	//PositionedModel* positionedBoxModel = new PositionedModel(boxModel, box->GetPosition(), box->GetRotation());
	//scene->models.push_back(positionedBoxModel);

	Model* busModel = ModelReader::Read("Models\\ikarus_260.obj");
	//PositionedModel* positionedBusModel = new PositionedModel(busModel, bus->GetPosition(), bus->GetRotation());
	//scene->models.push_back(positionedBusModel)

	// the main loop that iterates throughout the game
	while (!window.ShouldClose())
	{
		window.PollEvents();

		// Simulate physics
		float elapsedTime = window.GetElapsedTime();
		cout << elapsedTime << endl;
		bus->Update(1 / 60.0f);
		physics->Simulate(1 / 60.0f);

		// Update
		//positionedBoxModel->position = box->GetPosition();
		//positionedBoxModel->rotation = box->GetRotation();

		// Draw scene
		window.Draw(scene);

		window.SwapBuffers();
	}

	delete scene;
}