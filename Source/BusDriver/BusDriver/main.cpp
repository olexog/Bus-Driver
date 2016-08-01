#include <glm\vec3.hpp>

#include "Window.h"
#include "ShaderProgram.h"
#include "ModelReader.h"
#include "SceneReader.h"
#include "VertexArray.h"
#include "ColouredVertexArray.h"

#include "Physics.h"
#include "PhysicsScene.h"
#include "StaticActor.h"
#include "DynamicActor.h"
#include "Vehicle.h"

using namespace GraphicsLibrary;
using namespace PhysicsLibrary;
using namespace std;
using namespace glm;

int main()
{
	// create the physics environment
	Physics* physics = new Physics();

	// initialize the physics scene
	PhysicsScene* physicsScene = new PhysicsScene(physics);

	// add a moving box to the scene
	DynamicActor* box = new DynamicActor(physics, vec3(1.0f, 1.0f, 1.0f), vec3(0, 5, 0));
	physicsScene->AddActor(box);

	// add a movable vehicle to the scene
	Vehicle* bus = new Vehicle(physics);
	physicsScene->AddActor(bus);
	bus->SetToRestState();

	// create the window
	Window window = Window(640, 480, "Bus Driver");

	Scene* scene = SceneReader::Read("Models\\simple.map");

	Model* boxModel = ModelReader::Read("Models\\box.obj");
	PositionedModel* positionedBoxModel = new PositionedModel(boxModel, box->GetPosition(), box->GetRotation());
	scene->models.push_back(positionedBoxModel);

	vector<vector<vec3>> wheelVertices = bus->GetWheelVertices();
	vector<Model*> physicsWheelModels;
	for (int i = 0; i < wheelVertices.size(); i++)
	{
		physicsWheelModels.push_back(new Model(new ColouredVertexArray(wheelVertices[i], wheelVertices[i], wheelVertices[i], wheelVertices[i], wheelVertices[i])));
	}
	Model* wheelModel = ModelReader::Read("Models\\ikarus_260_wheel.obj");

	vector<vec3> chassisVertices = bus->GetChassisVertices();
	Model* physicsChassisModel = new Model(new ColouredVertexArray(chassisVertices, chassisVertices, chassisVertices, chassisVertices, chassisVertices));
	Model* chassisModel = ModelReader::Read("Models\\ikarus_260_body.obj");

	// the main loop that iterates throughout the game
	while (!window.ShouldClose())
	{
		window.PollEvents();

		if (window.IsPressed(GLFW_KEY_W))
		{
			bus->Accelerate(1.0f);
		}
		else
		{
			bus->Accelerate(0.0f);
		}

		if (window.IsPressed(GLFW_KEY_S))
		{
			bus->Brake(1.0f);
		}
		else
		{
			bus->Brake(0.0f);
		}

		if (window.IsPressed(GLFW_KEY_A) && !window.IsPressed(GLFW_KEY_D))
		{
			bus->Turn(1.0f);
		}
		else if (window.IsPressed(GLFW_KEY_D) && !window.IsPressed(GLFW_KEY_A))
		{
			bus->Turn(-1.0f);
		}
		else
		{
			bus->Turn(0.0f);
		}

		if (window.IsPressed(GLFW_KEY_SPACE))
		{
			bus->Handbrake(1.0f);
		}
		else
		{
			bus->Handbrake(0.0f);
		}

		if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE)
		{
			vector<float> axes = window.GetAxes();
			
			bus->Turn(-axes[0]);
			
			if (axes[1] < 0)
			{
				bus->Accelerate(-axes[1]);
			}
			else
			{
				bus->Brake(axes[1]);
			}
		}

		// simulate physics
		float elapsedTime = window.GetElapsedTime();
		physicsScene->Simulate(elapsedTime);

		// draw scene
		window.Draw(scene, bus->GetPosition(), bus->GetRotation(), physicsWheelModels, bus->GetWheelPositions(), bus->GetWheelRotations(), physicsChassisModel, bus->GetChassisPosition(), bus->GetChassisRotation(), wheelModel, chassisModel);

		window.SwapBuffers();
	}

	delete scene;
}