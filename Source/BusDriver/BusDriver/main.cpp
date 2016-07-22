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

using namespace GraphicsLibrary;
using namespace PhysicsLibrary;
using namespace std;
using namespace glm;

int main()
{
	// creates the physics environment
	Physics* physics = new Physics();

	// creates the window
	Window window = Window(640, 480, "Bus Driver");

	//Model* model = ModelReader::Read("Models\\station_01.obj");

	Scene* scene = SceneReader::Read("Models\\simple.map");

	// the main loop that iterates throughout the game
	while (!window.ShouldClose())
	{
		window.PollEvents();

		window.Draw(scene);

		window.SwapBuffers();
	}

	delete scene;
}