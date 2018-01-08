#include <RakPeerInterface.h>

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
#include "TextDrawing.h"

#include "DrivenThingy.h"

// TO DELETE
#include "VehicleCreate.h"

using namespace GraphicsLibrary;
using namespace PhysicsLibrary;
using namespace std;
using namespace glm;
using namespace RakNet;

GLFWwindow* glfwWindow;

OpenGl* openGl;

bool firstMouse = true;
vec2 cursorPreviousPosition;

FreeCamera* freeCamera;
FollowCamera* followCamera;

vector<Camera*> cameras;
int activeCameraIndex;

bool isPaused;

bool pressedKeys[GLFW_KEY_LAST];

const float MAX_FRAME_LENGTH = 0.016667f;

float previousTime;
vec3 busPreviousPosition;

TextDrawing* activeCameraText;
TextDrawing* velocityText;

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

	// Escape: pause/continue
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (isPaused)
		{
			isPaused = false;
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			isPaused = true;
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
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

		activeCameraText->Text = "Active camera: " + to_string(activeCameraIndex);
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
	RakPeerInterface* peer = RakPeerInterface::GetInstance();

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

	Scene* scene = map.CreateScene();

	Playground* playground = map.CreatePlayground(physics);

	vector<vec3> ikarusWheelVertices;
	Model* ikarusWheelModel = ModelReader::Read("Models\\ikarus_260_wheel.obj", ikarusWheelVertices);
	Model* ikarusChassisModel = ModelReader::Read("Models\\ikarus_260_body.obj");
	DrivenThingy* ikarus = new DrivenThingy(ikarusWheelVertices, physics, 2.5f, 3.4f, 11.0f, ikarusWheelModel, ikarusChassisModel,
		1500.0f, 20.0f, 0.704f, 0.748f, 0.7f, 2.7f, 2.7f, PxVec3(-2, 3, 22), playground);
	ikarus->AddToScene(scene);

	vector<vec3> ikarus2WheelVertices;
	Model* ikarus2WheelModel = ModelReader::Read("Models\\ikarus_260_wheel.obj", ikarus2WheelVertices);
	Model* ikarus2ChassisModel = ModelReader::Read("Models\\ikarus_260_body.obj");
	DrivenThingy* ikarus2 = new DrivenThingy(ikarus2WheelVertices, physics, 2.5f, 3.4f, 11.0f, ikarus2WheelModel, ikarus2ChassisModel,
		1500.0f, 20.0f, 0.704f, 0.748f, 0.7f, 2.7f, 2.7f, PxVec3(2, 3, 22), playground);
	ikarus2->AddToScene(scene);

	vector<vec3> trabantWheelVertices;
	Model* trabantWheelModel = ModelReader::Read("Models\\trabant_wheel.obj", trabantWheelVertices);
	Model* trabantChassisModel = ModelReader::Read("Models\\trabant_body.obj");
	DrivenThingy* trabant = new DrivenThingy(trabantWheelVertices, physics, 1.51f, 1.437f, 3.56f, trabantWheelModel, trabantChassisModel,
		650.0f, 20.0f, 0.35f, 0.116f, 0.432f, 1.182f, 0.835f, PxVec3(-2.3, 3, 6), playground);
	trabant->AddToScene(scene);
	DrivenThingy* trabant2 = new DrivenThingy(trabantWheelVertices, physics, 1.51f, 1.437f, 3.56f, trabantWheelModel, trabantChassisModel,
		650.0f, 20.0f, 0.35f, 0.116f, 0.432f, 1.182f, 0.835f, PxVec3(2.3, 3, 6), playground);
	trabant2->AddToScene(scene);

	//pointer to the driven vehicle
	Vehicle* vehicle = trabant2->GetVehicle();

	// initialize camera

	freeCamera = new FreeCamera(vec3(0.0f, 3.0f, 0.0f), 0.0f, 0.0f);
	followCamera = new FollowCamera(vehicle->GetPosition(), vehicle->GetRotation());

	cameras = vector<Camera*>();
	cameras.push_back(freeCamera);
	cameras.push_back(followCamera);
	activeCameraIndex = 1;

	activeCameraText = new TextDrawing("Active camera: " + to_string(activeCameraIndex), 1.0f, HorizontalAlignment::Right, VerticalAlignment::Top,
		vec2(40.0f, 20.0f), vec3(1.0f, 0.0f, 0.0f));
	velocityText = new TextDrawing("0 km/h", 1.0f, HorizontalAlignment::Left, VerticalAlignment::Bottom, vec2(20.0f), vec3(0.0f, 0.0f, 1.0f));

	openGl->AddTextDrawing(activeCameraText);
	openGl->AddTextDrawing(velocityText);

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
		SetControlls(vehicle);

		// simulate physics
		playground->Simulate(elapsedTime);

		// update driven thingies
		ikarus->Update();
		ikarus2->Update();
		trabant->Update();
		trabant2->Update();

		// update the camera
		UpdateCamera(elapsedTime, vehicle);

		// transfer the camera pose to the renderer

		openGl->SetCameraStatic(freeCamera->GetPosition(), freeCamera->GetFront());
		openGl->SetCameraDynamic(followCamera->GetPosition(), followCamera->GetFront());

		// calculate bus velocity
		float velocity = length(vehicle->GetPosition() - busPreviousPosition) / elapsedTime;
		busPreviousPosition = vehicle->GetPosition();

		// update texts
		velocityText->Text = "Velocity: " + to_string(static_cast<int>(velocity * 3.6f)) + " km/h";

		// draw scene
		openGl->Draw(scene);

		// swap the screen buffers
		glfwSwapBuffers(glfwWindow);
	}

	// call the destructors
	delete scene;
	delete openGl;

	// terminate the GLFW context
	glfwTerminate();
}