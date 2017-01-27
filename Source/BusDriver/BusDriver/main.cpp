#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "OpenGl.h"

// GLFW must be included after gl.h - which is in OpenGl.h
#include <GLFW/glfw3.h>

#include "MapReader.h"

#include "ModelReader.h"
#include "ShaderProgram.h"
#include "VertexArray.h"

#include "Physics.h"
#include "Playground.h"
#include "DynamicActor.h"
#include "Vehicle.h"
#include "PhysicsUtility.h"
#include "FreeCamera.h"
#include "FollowCamera.h"

// TO DELETE
#include "VehicleCreate.h"

using namespace GraphicsLibrary;
using namespace PhysicsLibrary;
using namespace std;
using namespace glm;

GLFWwindow* glfwWindow;

OpenGl* openGl;

bool firstMouse = true;
vec2 cursorPreviousPosition;

FreeCamera* freeCamera;
FollowCamera* followCamera;

vector<Camera*> cameras;
int activeCameraIndex;

bool pressedKeys[GLFW_KEY_LAST];

const float MAX_FRAME_LENGTH = 0.016667f;

float previousTime;
vec3 busPreviousPosition;

void WindowSize(GLFWwindow* window, int width, int height)
{
	// set the OpenGL viewport
	if (openGl != NULL)
	{
		openGl->SetContextSize(width, height);
	}
}

void Key(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// handle invalid keys
	if (key < 0)
	{
		return;
	}

	// Escape: exit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(glfwWindow, GL_TRUE);
	}
	// M: wireframe mode
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		// set wireframe mode to the renderer
		if (openGl != NULL)
		{
			openGl->SetWireframeMode(true);
		}
	}
	else if (key == GLFW_KEY_M && action == GLFW_RELEASE)
	{
		// set to normal rendering mode
		if (openGl != NULL)
		{
			openGl->SetWireframeMode(false);
		}
	}
	// X: change camera mode
	else if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		if (++activeCameraIndex == cameras.size())
		{
			activeCameraIndex = 0;
			openGl->SetCameraMode(true);
		}
		else
		{
			openGl->SetCameraMode(false);
		}
	}
	// CTRL + L: light's view
	else if (pressedKeys[GLFW_KEY_LEFT_CONTROL] && key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (openGl != NULL)
		{
			openGl->SetViewFromLight(true);
		}
	}
	else if (key == GLFW_KEY_L && action == GLFW_RELEASE)
	{
		if (openGl != NULL)
		{
			openGl->SetViewFromLight(false);
		}

		pressedKeys[GLFW_KEY_L] = false;
	}
	// 0, 1, 2: set actual cascade
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		if (openGl != NULL)
		{
			openGl->SetCascadeToVisualize(0);
		}
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		if (openGl != NULL)
		{
			openGl->SetCascadeToVisualize(1);
		}
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		if (openGl != NULL)
		{
			openGl->SetCascadeToVisualize(2);
		}
	}
	// C: render shadow map
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		if (openGl != NULL)
		{
			openGl->SetRenderShadowMap(true);
		}
	}
	else if (key == GLFW_KEY_C && action == GLFW_RELEASE)
	{
		if (openGl != NULL)
		{
			openGl->SetRenderShadowMap(false);
		}
	}
	// store all the other keys' state
	else if (action == GLFW_PRESS)
	{
		pressedKeys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		pressedKeys[key] = false;
	}
}

void CursorPos(GLFWwindow* window, double xpos, double ypos)
{
	vec2 cursorPosition = vec2(xpos, ypos);

	if (firstMouse)
	{
		cursorPreviousPosition = cursorPosition;
		firstMouse = false;
	}

	vec2 offset = cursorPosition - cursorPreviousPosition;

	cursorPreviousPosition = cursorPosition;

	freeCamera->RotateWithMouse(offset.x, offset.y, true);
}

void SetControlls(Vehicle* vehicle)
{
	if (pressedKeys[GLFW_KEY_W])
	{
		vehicle->Accelerate(1.0f);
	}
	else
	{
		vehicle->Accelerate(0.0f);
	}

	if (pressedKeys[GLFW_KEY_S])
	{
		vehicle->Brake(1.0f);
	}
	else
	{
		vehicle->Brake(0.0f);
	}

	if (pressedKeys[GLFW_KEY_A] && !pressedKeys[GLFW_KEY_D])
	{
		vehicle->Turn(1.0f);
	}
	else if (pressedKeys[GLFW_KEY_D] && !pressedKeys[GLFW_KEY_A])
	{
		vehicle->Turn(-1.0f);
	}
	else
	{
		vehicle->Turn(0.0f);
	}

	if (pressedKeys[GLFW_KEY_SPACE])
	{
		vehicle->Handbrake(1.0f);
	}
	else
	{
		vehicle->Handbrake(0.0f);
	}

	if (pressedKeys[GLFW_KEY_F])
	{
		vehicle->SetForwardGear();
	}
	else if (pressedKeys[GLFW_KEY_R])
	{
		vehicle->SetReverseGear();
	}

	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE)
	{
		int count;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
		vector<float> result;

		for (int i = 0; i < count; i++)
		{
			result.push_back(axes[i]);
		}

		vehicle->Turn(-axes[0]);

		if (axes[1] < 0)
		{
			vehicle->Accelerate(-axes[1]);
		}
		else
		{
			vehicle->Brake(axes[1]);
		}
	}
}

void UpdateCamera(float elapsedTime, Vehicle* vehicle)
{
	// set fast mode
	bool fastModeOn = pressedKeys[GLFW_KEY_B];
	freeCamera->SetFastMode(fastModeOn);

	// set static camera
		// translate up
	if (pressedKeys[GLFW_KEY_I])
	{
		freeCamera->Move(CameraMovement::Up, elapsedTime);
	}
	// translate down
	if (pressedKeys[GLFW_KEY_K])
	{
		freeCamera->Move(CameraMovement::Down, elapsedTime);
	}
	// translate left
	if (pressedKeys[GLFW_KEY_J])
	{
		freeCamera->Move(CameraMovement::Left, elapsedTime);
	}
	// translate right
	if (pressedKeys[GLFW_KEY_L])
	{
		freeCamera->Move(CameraMovement::Right, elapsedTime);
	}
	// translate forward
	if (pressedKeys[GLFW_KEY_O])
	{
		freeCamera->Move(CameraMovement::Forward, elapsedTime);
	}
	// translate backward
	if (pressedKeys[GLFW_KEY_U])
	{
		freeCamera->Move(CameraMovement::Backward, elapsedTime);
	}

	// set bus camera
	followCamera->FollowTarget(vehicle->GetPosition(), vehicle->GetRotation(), elapsedTime);
}

int main()
{
	// initialize GLFW
	glfwInit();

	// assert OpenGL versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window
	glfwWindow = glfwCreateWindow(640, 480, "Bus Driver", NULL, NULL);

	// check the successfulness of the window creation
	if (glfwWindow == NULL)
	{
		std::cerr << "Failed to create GLFW window." << std::endl;
		glfwTerminate();

		return 0;
	}

	// select the window
	glfwMakeContextCurrent(glfwWindow);

	// capture cursor
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// callbacks
	glfwSetWindowSizeCallback(glfwWindow, WindowSize);
	glfwSetKeyCallback(glfwWindow, Key);
	glfwSetCursorPosCallback(glfwWindow, CursorPos);

	// create the window
	openGl = new OpenGl(640, 480);

	// setup window parameters
	openGl->SetCascadeToVisualize(0);

	// create the physics environment
	Physics* physics = new Physics();

	Map map = MapReader::Read(physics, "Models\\city.map");

	vector<vec3> wheelVertices;
	Model* wheelModel = ModelReader::Read("Models\\ikarus_260_wheel.obj", wheelVertices);

	vector<vec3*> wheelPositions = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelOrientations = { new quat(), new quat(), new quat(), new quat() };
	vector<vec3*> wheelLocalPositions = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelLocalOrientations = { new quat(), new quat(), new quat(), new quat() };
	vector<Shape*> wheels;

	for (int i = 0; i < 4; i++)
	{
		Shape* wheel = PhysicsUtility::ShapeFromConvexTriangles(wheelVertices, physics);
		wheel->SetType(Shape::Type::WHEEL);
		wheel->SetPosePointer(wheelLocalPositions[i], wheelLocalOrientations[i]);
		
		wheels.push_back(wheel);
	}

	vec3* chassisPosition = new vec3();
	quat* chassisOrientation = new quat();
	vec3* chassisLocalPosition = new vec3(0.0f, -0.5f, 0.0f);
	quat* chassisLocalOrientation = new quat();

	PxConvexMesh* chassisMesh = createChassisMesh(PxVec3(2.5f, 3.4f, 11.0f), *physics->GetPhysics(), *physics->GetCooking());
	PxConvexMeshGeometry* chassisGeometry = new PxConvexMeshGeometry(chassisMesh);
	Shape* chassis = new Shape(physics, chassisGeometry, chassisLocalPosition, chassisLocalOrientation, Shape::Type::CHASSIS);

	Model* chassisModel = ModelReader::Read("Models\\ikarus_260_body.obj");

	Scene* scene = map.CreateScene();
	for (int i = 0; i < 4; i++)
	{
		scene->models.push_back(new PositionedModel(wheelModel, wheelPositions[i], wheelOrientations[i]));
	}
	scene->models.push_back(new PositionedModel(chassisModel, chassisPosition, chassisOrientation));

	Playground* playground = map.CreatePlayground(physics);

	// add a movable vehicle to the scene
	Vehicle* bus = new Vehicle(physics, chassis, wheels);
	playground->AddActor(bus);
	bus->SetToRestState();

	// initialize camera

	freeCamera = new FreeCamera(vec3(0.0f, 3.0f, -100.0f), 0.0f, 0.0f);
	followCamera = new FollowCamera(bus->GetPosition(), bus->GetRotation());

	cameras = vector<Camera*>();
	cameras.push_back(freeCamera);
	cameras.push_back(followCamera);
	activeCameraIndex = 1;

	// the main loop that iterates throughout the game
	while (glfwWindowShouldClose(glfwWindow) != GL_TRUE)
	{
		// poll events from the window
		glfwPollEvents();

		// calculate elapsed time
		float elapsedTime;
		float totalTime = static_cast<float>(glfwGetTime());

		if (previousTime == 0.0f)
		{
			elapsedTime = FLT_EPSILON;
		}
		else
		{
			elapsedTime = glm::min(totalTime - previousTime, MAX_FRAME_LENGTH);
		}

		previousTime = totalTime;

		// controll the bus
		SetControlls(bus);

		// simulate physics
		playground->Simulate(elapsedTime);
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
			globalTranslation = translate(globalTranslation, bus->GetPosition());
			mat4 globalRotation = static_cast<mat4>(bus->GetRotation());
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
			globalTranslation = translate(globalTranslation, bus->GetPosition());
			mat4 globalRotation = static_cast<mat4>(bus->GetRotation());
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

		// update the camera
		UpdateCamera(elapsedTime, bus);

		// transfer the camera pose to the renderer

		openGl->SetCameraStatic(freeCamera->GetPosition(), freeCamera->GetFront());
		openGl->SetCameraDynamic(followCamera->GetPosition(), followCamera->GetFront());

		// draw scene
		openGl->Draw(scene);

		float velocity = length(bus->GetPosition() - busPreviousPosition) / elapsedTime;
		busPreviousPosition = bus->GetPosition();

		openGl->DrawText("Active camera: " + to_string(activeCameraIndex), 1.0f, HorizontalAlignment::Right, VerticalAlignment::Top, vec2(40.0f, 20.0f), vec3(1.0f, 0.0f, 0.0f));
		openGl->DrawText("Velocity: " + to_string(static_cast<int>(velocity * 3.6f)) + " km/h", 1.0f, HorizontalAlignment::Left, VerticalAlignment::Bottom, vec2(20.0f), vec3(0.0f, 0.0f, 1.0f));

		// swap the screen buffers
		glfwSwapBuffers(glfwWindow);
	}

	// call the destructors
	delete scene;
	delete openGl;

	// terminate the GLFW context
	glfwTerminate();
}