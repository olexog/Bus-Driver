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
#include "ColouredVertexArray.h"

#include "Physics.h"
#include "Playground.h"
#include "DynamicActor.h"
#include "Vehicle.h"
#include "PhysicsUtility.h"

// TO DELETE
#include "VehicleCreate.h"

using namespace GraphicsLibrary;
using namespace PhysicsLibrary;
using namespace std;
using namespace glm;

GLFWwindow* glfwWindow;

OpenGl* openGl;

bool pressedKeys[GLFW_KEY_LAST];

enum class CameraMode
{
	Static,
	FollowBus
} cameraMode;

const float CAMERA_VELOCITY = 20.0f;
const float CAMERA_ANGULAR_VELOCITY = 0.5f;
const float CAMERA_FAST_VELOCITY = 100.0f;
const float CAMERA_FAST_ANGULAR_VELOCITY = 1.0f;

vec3 cameraPosition;
vec3 cameraDirection;

float previousTime;

void WindowSize(GLFWwindow* window, int width, int height)
{
	// set the OpenGL viewport
	if (openGl != NULL)
	{
		openGl->SetViewport(width, height);
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
		switch (cameraMode)
		{
		case CameraMode::FollowBus:
			cameraMode = CameraMode::Static;
			break;
		case CameraMode::Static:
			cameraMode = CameraMode::FollowBus;
			break;
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
	// static camera
	if (cameraMode == CameraMode::Static)
	{
		vec3 up = vec3(0.0f, 1.0f, 0.0f);

		// define camera velocity / angular velocity
		float cameraVelocity = pressedKeys[GLFW_KEY_LEFT_SHIFT] ? CAMERA_FAST_VELOCITY : CAMERA_VELOCITY;
		float cameraAngularVelocity = pressedKeys[GLFW_KEY_LEFT_SHIFT] ? CAMERA_FAST_ANGULAR_VELOCITY : CAMERA_ANGULAR_VELOCITY;

		// rotate up
		if (pressedKeys[GLFW_KEY_I])
		{
			cameraDirection = vec3(glm::rotate(mat4(), cameraAngularVelocity * elapsedTime, cross(cameraDirection, up)) * vec4(cameraDirection, 0.0f));
		}
		// rotate down
		if (pressedKeys[GLFW_KEY_K])
		{
			cameraDirection = vec3(glm::rotate(mat4(), cameraAngularVelocity * elapsedTime, -cross(cameraDirection, up)) * vec4(cameraDirection, 0.0f));
		}
		// rotate left
		if (pressedKeys[GLFW_KEY_J])
		{
			cameraDirection = vec3(glm::rotate(mat4(), cameraAngularVelocity * elapsedTime, up) * vec4(cameraDirection, 0.0f));
		}
		// rotate right
		if (pressedKeys[GLFW_KEY_L])
		{
			cameraDirection = vec3(glm::rotate(mat4(), cameraAngularVelocity * elapsedTime, -up) * vec4(cameraDirection, 0.0f));
		}
		// translate up
		if (pressedKeys[GLFW_KEY_KP_8])
		{
			cameraPosition += cameraVelocity * elapsedTime * up;
		}
		// translate down
		if (pressedKeys[GLFW_KEY_KP_2])
		{
			cameraPosition -= cameraVelocity * elapsedTime * up;
		}
		// translate left
		if (pressedKeys[GLFW_KEY_KP_4])
		{
			cameraPosition -= cameraVelocity * elapsedTime * cross(cameraDirection, up);
		}
		// translate right
		if (pressedKeys[GLFW_KEY_KP_6])
		{
			cameraPosition += cameraVelocity * elapsedTime * cross(cameraDirection, up);
		}
		// translate forward
		if (pressedKeys[GLFW_KEY_KP_9])
		{
			cameraPosition += cameraVelocity * elapsedTime * cameraDirection;
		}
		// translate backward
		if (pressedKeys[GLFW_KEY_KP_7])
		{
			cameraPosition -= cameraVelocity * elapsedTime * cameraDirection;
		}
	}
	// bus camera
	else if (cameraMode == CameraMode::FollowBus)
	{
		cameraPosition = vec3(0.0f, 5.0f, -20.0f);
		cameraPosition = glm::rotate(vehicle->GetRotation(), cameraPosition) + vehicle->GetPosition();
		cameraDirection = vec3(0.0f, 0.0f, 1.0f);
		cameraDirection = glm::rotate(vehicle->GetRotation(), cameraDirection);
	}
}

int main()
{
	cameraMode = CameraMode::FollowBus;

	if (cameraMode == CameraMode::Static)
	{
		cameraPosition = vec3(0.0f, 3.0f, -100.0f);
		cameraDirection = vec3(0.0f, 0.0f, 1.0f);
	}
	else if (cameraMode == CameraMode::FollowBus)
	{

	}

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

	// callbacks
	glfwSetWindowSizeCallback(glfwWindow, WindowSize);
	glfwSetKeyCallback(glfwWindow, Key);

	// create the window
	openGl = new OpenGl(640, 480);

	// create the physics environment
	Physics* physics = new Physics();

	Map map = MapReader::Read(physics, "Models\\kresz_park.map");

	vector<vec3> wheelVertices;
	Model* wheelModel = ModelReader::Read("Models\\ikarus_260_wheel.obj", wheelVertices);

	//PxConvexMesh* wheelMesh = createWheelMesh(0.748f, 0.704f, *physics->GetPhysics(), *physics->GetCooking());
	//PxConvexMeshGeometry* geom = new PxConvexMeshGeometry(wheelMesh);
	//Shape* wheel = new Shape(physics, geom, PxVec3(PxIdentity), PxQuat(PxIdentity), Shape::Type::WHEEL);
	//Shape* wheel = PhysicsUtility::ShapeFromConvexTriangles(wheelVertices, physics);
	//wheel->SetType(Shape::Type::WHEEL);
	//Body* wheelBody = new Body({ wheel });

	vector<vec3*> wheelPositions = { new vec3(), new vec3(), new vec3(), new vec3() };
	vector<quat*> wheelOrientations = { new quat(), new quat(), new quat(), new quat() };
	vector<Shape*> wheels;

	for (int i = 0; i < 4; i++)
	{
		Shape* wheel = PhysicsUtility::ShapeFromConvexTriangles(wheelVertices, physics);
		wheel->SetType(Shape::Type::WHEEL);
		wheel->SetPosePointer(wheelPositions[i], wheelOrientations[i]);
		
		wheels.push_back(wheel);
	}

	vec3* chassisPosition = new vec3(0.0f, -0.5f, 0.0f);
	quat* chassisOrientation = new quat();

	PxConvexMesh* chassisMesh = createChassisMesh(PxVec3(2.5f, 3.4f, 11.0f), *physics->GetPhysics(), *physics->GetCooking());
	PxConvexMeshGeometry* chassisGeometry = new PxConvexMeshGeometry(chassisMesh);
	Shape* chassis = new Shape(physics, chassisGeometry, chassisPosition, chassisOrientation, Shape::Type::CHASSIS);

	//vector<vector<vec3>> wheelVertices = bus->GetWheelVertices();
	vector<Model*> physicsWheelModels;
	for (int i = 0; i < wheelVertices.size(); i++)
	{
		//physicsWheelModels.push_back(new Model(new ColouredVertexArray(wheelVertices[i], wheelVertices[i], wheelVertices[i], wheelVertices[i], wheelVertices[i])));
	}
	//Model* wheelModel = ModelReader::Read("Models\\ikarus_260_wheel.obj");

	//vector<vec3> chassisVertices = bus->GetChassisVertices();
	//Model* physicsChassisModel = new Model(new ColouredVertexArray(chassisVertices, chassisVertices, chassisVertices, chassisVertices, chassisVertices));
	Model* chassisModel = ModelReader::Read("Models\\ikarus_260_body.obj");

	Scene* scene = map.CreateScene();
	//for (int i = 0; i < 4; i++)
	//{
	//	scene->models.push_back(new PositionedModel(wheelModel, wheelPositions[i], wheelOrientations[i]));
	//}
	//scene->models.push_back(new PositionedModel(chassisModel, chassisPosition, chassisOrientation));

	Playground* playground = map.CreatePlayground(physics);

	// add a movable vehicle to the scene
	Vehicle* bus = new Vehicle(physics, chassis, wheels);
	playground->AddActor(bus);
	bus->SetToRestState();

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
			elapsedTime = totalTime - previousTime;
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

		// update the camera
		UpdateCamera(elapsedTime, bus);

		// transfer the camera pose to the renderer
		openGl->SetCamera(cameraPosition, cameraDirection);

		// draw scene
		openGl->Draw(scene, bus->GetPosition(), bus->GetRotation(), wheelPositions, wheelOrientations, chassisPosition, chassisOrientation, wheelModel, chassisModel);

		// swap the screen buffers
		glfwSwapBuffers(glfwWindow);
	}

	// call the constructors
	delete scene;
	delete openGl;

	// terminate the GLFW context
	glfwTerminate();
}