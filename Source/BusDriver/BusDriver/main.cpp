#include <map>
#include <vector>
#include <fstream>

#include <PxPhysicsAPI.h>

#include "Window.h"
#include "ShaderProgram.h"
#include "ModelReader.h"
#include "VertexArray.h"

using namespace GraphicsLibrary;
using namespace std;

int main()
{
	// creates the window
	Window window = Window(640, 480, "Bus Driver");

	//Model* loadedModel;

	//ModelReader reader = ModelReader();
	//reader.Read("Models\\station_01.obj", loadedModel);

	VertexArray* vertexArray = new VertexArray({ PxVec3(-0.5f, -0.5f, 0.0f), PxVec3(0.5f, -0.5f, 0.0f), PxVec3(0.0f,  0.5f, 0.0f) });

	//Model* model = new Model({ collection });

	// the main loop that iterates throughout the game
	while (!window.ShouldClose())
	{
		window.PollEvents();

		window.Draw(vertexArray);

		window.SwapBuffers();
	}

	delete vertexArray;
}