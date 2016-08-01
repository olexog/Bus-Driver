#include "StaticActor.h"

namespace PhysicsLibrary
{
	StaticActor::StaticActor(Physics* physics, vector<vec3> points, vec3 position, quat orientation)
	{
		PxVec3 PxPosition = PxVec3(position.x, position.y, position.z);
		PxQuat PxOrientation = PxQuat(orientation.x, orientation.y, orientation.z, orientation.w);
		this->actor = physics->GetPhysics()->createRigidStatic(PxTransform(PxPosition, PxOrientation));

		vector<PxU32> indices;
		for (int i = 0; i < points.size(); i++)
		{
			indices.push_back(i);
		}

		PxTriangleMesh* mesh;
		PxTriangleMeshDesc description;
		description.points.count = static_cast<PxU32>(points.size());
		description.points.data = points.data();
		description.points.stride = sizeof(PxVec3);
		description.triangles.count = static_cast<PxU32>(indices.size());
		description.triangles.data = indices.data();
		description.triangles.stride = sizeof(PxU32) * 3;

		PxDefaultMemoryOutputStream writeBuffer;
		bool status = physics->GetCooking()->cookTriangleMesh(description, writeBuffer);

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		mesh = physics->GetPhysics()->createTriangleMesh(readBuffer);

		PxTriangleMeshGeometry geometry = PxTriangleMeshGeometry(mesh);

		PxShape* cubeShape = this->actor->createShape(geometry, *physics->GetMaterial(), PxTransform(PxIdentity));
		cubeShape->setSimulationFilterData(physics->obstacleSimFilterData);
	}

	void StaticActor::AddToScene(PxScene* scene)
	{
		scene->addActor(*this->actor);
	}

	StaticActor::~StaticActor()
	{
	}
}
