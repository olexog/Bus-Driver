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

	vector<vec3> wheelVertices2;
	Model* trabantWheelModel = ModelReader::Read("Models\\trabant_wheel.obj", wheelVertices2);

	vector<vec3*> wheelPositions = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelOrientations = { new quat(), new quat(), new quat(), new quat() };
	vector<vec3*> wheelLocalPositions = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelLocalOrientations = { new quat(), new quat(), new quat(), new quat() };
	vector<Shape*> wheels;

	vector<vec3*> wheelPositions2 = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelOrientations2 = { new quat(), new quat(), new quat(), new quat() };
	vector<vec3*> wheelLocalPositions2 = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelLocalOrientations2 = { new quat(), new quat(), new quat(0.7071f, 0.7071f, 0.0f, 0.0f), new quat(0.7071f, 0.7071f, 0.0f, 0.0f) };
	vector<Shape*> wheels2;

	for (int i = 0; i < 4; i++)
	{
		Shape* wheel = PhysicsUtility::ShapeFromConvexTriangles(wheelVertices, physics);
		wheel->SetType(Shape::Type::WHEEL);
		wheel->SetPosePointer(wheelLocalPositions[i], wheelLocalOrientations[i]);
		
		wheels.push_back(wheel);

		Shape* wheel2 = PhysicsUtility::ShapeFromConvexTriangles(wheelVertices2, physics);
		wheel2->SetType(Shape::Type::WHEEL);
		wheel2->SetPosePointer(wheelLocalPositions2[i], wheelLocalOrientations2[i]);

		wheels2.push_back(wheel2);
	}

	vec3* chassisPosition = new vec3();
	quat* chassisOrientation = new quat();
	vec3* chassisLocalPosition = new vec3(0.0f, -0.5f, 0.0f);
	quat* chassisLocalOrientation = new quat();

	vec3* chassisPosition2 = new vec3();
	quat* chassisOrientation2 = new quat();
	vec3* chassisLocalPosition2 = new vec3(0.0f, 0.0f, 0.0f);
	quat* chassisLocalOrientation2 = new quat();

	PxConvexMesh* chassisMesh = createChassisMesh(PxVec3(2.5f, 3.4f, 11.0f), *physics->GetPhysics(), *physics->GetCooking());
	PxConvexMeshGeometry* chassisGeometry = new PxConvexMeshGeometry(chassisMesh);
	PxConvexMesh* chassisMesh2 = createChassisMesh(PxVec3(1.51f, 1.437f, 3.56f), *physics->GetPhysics(), *physics->GetCooking());
	PxConvexMeshGeometry* chassisGeometry2 = new PxConvexMeshGeometry(chassisMesh2);
	Shape* chassis = new Shape(physics, chassisGeometry, chassisLocalPosition, chassisLocalOrientation, Shape::Type::CHASSIS);
	Shape* chassis2 = new Shape(physics, chassisGeometry2, chassisLocalPosition2, chassisLocalOrientation2, Shape::Type::CHASSIS);

	Model* chassisModel = ModelReader::Read("Models\\ikarus_260_body.obj");
	Model* chassisModel2 = ModelReader::Read("Models\\trabant_body.obj");

	Scene* scene = map.CreateScene();
	for (int i = 0; i < 4; i++)
	{
		scene->models.push_back(new PositionedModel(wheelModel, wheelPositions[i], wheelOrientations[i]));
		scene->models.push_back(new PositionedModel(trabantWheelModel, wheelPositions2[i], wheelOrientations2[i]));
	}
	scene->models.push_back(new PositionedModel(chassisModel, chassisPosition, chassisOrientation));
	scene->models.push_back(new PositionedModel(chassisModel2, chassisPosition2, chassisOrientation2));

	Playground* playground = map.CreatePlayground(physics);

	vector<vec3> ikarusWheelVertices;
	Model* ikarusWheelModel = ModelReader::Read("Models\\ikarus_260_wheel.obj", ikarusWheelVertices);
	Model* ikarusChassisModel = ModelReader::Read("Models\\ikarus_260_body.obj");
	DrivenThingy* ikarus = new DrivenThingy(ikarusWheelVertices, physics, 2.5f, 3.4f, 11.0f, ikarusWheelModel, ikarusChassisModel, PxVec3(-2, 3, 22), playground);
	ikarus->AddToScene(scene);

	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f, 3.4f, 11.0f);
	const PxVec3 chassisMOI
	((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	const PxF32 trabantChassisMass = 650.0f;
	const PxVec3 trabantChassisDims(1.51f, 1.437f, 3.56f);
	const PxVec3 trabantChassisMOI
	((trabantChassisDims.y*trabantChassisDims.y + trabantChassisDims.z*trabantChassisDims.z)*trabantChassisMass / 12.0f,
		(trabantChassisDims.x*trabantChassisDims.x + trabantChassisDims.z*trabantChassisDims.z)*0.8f*trabantChassisMass / 12.0f,
		(trabantChassisDims.x*trabantChassisDims.x + trabantChassisDims.y*trabantChassisDims.y)*trabantChassisMass / 12.0f);
	const PxVec3 trabantChassisCMOffset(0.0f, -trabantChassisDims.y*0.5f + 0.65f, 0.25f);

	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.704f;
	const PxF32 wheelWidth = 0.748f;
	const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;

	const PxF32 trabantWheelMass = 20.0f;
	const PxF32 trabantWheelRadius = 0.35f;
	const PxF32 trabantWheelWidth = 0.116;
	const PxF32 trabantWheelMOI = 0.5f*trabantWheelMass*trabantWheelWidth*trabantWheelWidth;

	// the wheel base of the Ikarus 260
	const PxF32 wheelBase = 5.4f;
	// the difference between the bottom and the wheel's centre
	const PxF32 prolapse = 0.2f;

	// add a movable vehicle to the scene
	Vehicle* bus = new Vehicle(physics, chassis, wheels, PxVec3(2, 3, 22),
		chassisMass, chassisDims, chassisMOI, physics->GetMaterial(), chassisCMOffset,
		wheelMass, wheelRadius, wheelWidth, wheelMOI, physics->GetMaterial(), 4,
		2.7f, 2.7f, prolapse);
	playground->AddActor(bus);
	bus->SetToRestState();

	Vehicle* bus2 = new Vehicle(physics, chassis2, wheels2, PxVec3(2.3, 3, 6),
		trabantChassisMass, trabantChassisDims, trabantChassisMOI, physics->GetMaterial(), trabantChassisCMOffset,
		trabantWheelMass, trabantWheelRadius, trabantWheelWidth, trabantWheelMOI, physics->GetMaterial(), 4,
		1.182f, 0.835f, 0.432f);
	playground->AddActor(bus2);
	bus2->SetToRestState();

	// initialize camera

	freeCamera = new FreeCamera(vec3(0.0f, 3.0f, 0.0f), 0.0f, 0.0f);
	followCamera = new FollowCamera(bus2->GetPosition(), bus2->GetRotation());

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
		//pointer to the driven vehicle
		Vehicle* vehicle = ikarus->GetVehicle();

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

		ikarus->Update();

		for (Shape* wheel : wheels)
		{
			wheel->Update();
		}
		chassis->Update();
		for (Shape* wheel : wheels2)
		{
			wheel->Update();
		}
		chassis2->Update();

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
		for (int i = 0; i < wheelLocalPositions2.size(); i++)
		{
			mat4 localTranslation;
			localTranslation = translate(localTranslation, *wheelLocalPositions2[i]);
			mat4 localRotation = static_cast<mat4>(*wheelLocalOrientations2[i]);
			mat4 globalTranslation;
			globalTranslation = translate(globalTranslation, bus2->GetPosition());
			mat4 globalRotation = static_cast<mat4>(bus2->GetRotation());
			mat4 modelMatrix = globalTranslation * globalRotation * localTranslation * localRotation;
			vec3 position = vec3(modelMatrix[3]);
			quat orientation = static_cast<quat>(modelMatrix);

			wheelPositions2[i]->x = position.x;
			wheelPositions2[i]->y = position.y;
			wheelPositions2[i]->z = position.z;

			wheelOrientations2[i]->x = orientation.x;
			wheelOrientations2[i]->y = orientation.y;
			wheelOrientations2[i]->z = orientation.z;
			wheelOrientations2[i]->w = orientation.w;
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
		{
			mat4 localTranslation;
			localTranslation = translate(localTranslation, *chassisLocalPosition2);
			mat4 localRotation = static_cast<mat4>(*chassisLocalOrientation2);
			mat4 globalTranslation;
			globalTranslation = translate(globalTranslation, bus2->GetPosition());
			mat4 globalRotation = static_cast<mat4>(bus2->GetRotation());
			mat4 modelMatrix = globalTranslation * globalRotation * localTranslation * localRotation;

			vec3 position = vec3(modelMatrix[3]);
			quat orientation = static_cast<quat>(modelMatrix);

			chassisPosition2->x = position.x;
			chassisPosition2->y = position.y;
			chassisPosition2->z = position.z;

			chassisOrientation2->x = orientation.x;
			chassisOrientation2->y = orientation.y;
			chassisOrientation2->z = orientation.z;
			chassisOrientation2->w = orientation.w;
		}

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