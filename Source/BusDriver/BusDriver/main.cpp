#include <map>
#include <vector>
#include <fstream>

#include <glm\vec3.hpp>

#include "Window.h"
#include "ShaderProgram.h"
#include "ModelReader.h"
#include "VertexArray.h"
#include "ColouredVertexArray.h"

using namespace GraphicsLibrary;
using namespace std;
using namespace glm;

int main()
{
	// creates the window
	Window window = Window(640, 480, "Bus Driver");

	Model* model = ModelReader::Read("Models\\station_01.obj");

	VertexArray* vertexArray = new VertexArray({ vec3(-1.5f, -0.5f, 0.0f), vec3(-0.5f, -0.5f, 0.0f), vec3(-1.0f,  0.5f, 0.0f) });

	ColouredVertexArray* colouredVertexArray = new ColouredVertexArray({ vec3(0.5f, -0.5f, 0.0f), vec3(1.5f, -0.5f, 0.0f), vec3(1.0f,  0.5f, 0.0f) },
		{ vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f) });

	vector<VertexArray*> vertexArrays = vector<VertexArray*>();
	vertexArrays.push_back(vertexArray);
	vertexArrays.push_back(colouredVertexArray);

	// the main loop that iterates throughout the game
	while (!window.ShouldClose())
	{
		window.PollEvents();

		window.Draw(model);

		window.SwapBuffers();
	}

	delete vertexArray;
	delete model;
}