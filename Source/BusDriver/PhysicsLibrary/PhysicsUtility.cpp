#include "PhysicsUtility.h"

namespace PhysicsLibrary
{
	Body* PhysicsUtility::BodyFromTriangles(vector<vec3> vertices, Physics* physics)
	{
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = static_cast<PxU32>(vertices.size());
		meshDesc.points.stride = sizeof(vec3);
		meshDesc.points.data = vertices.data();

		vector<PxU32> indices = vector<PxU32>(0, vertices.size() - 1);

		meshDesc.triangles.count = static_cast<PxU32>(indices.size());
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = indices.data();

		PxDefaultMemoryOutputStream writeBuffer;
		bool success = physics->GetCooking()->cookTriangleMesh(meshDesc, writeBuffer);
		if (!success)
		{
			return NULL;
		}

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* mesh = physics->GetPhysics()->createTriangleMesh(readBuffer);

		return new Body({ new Shape(physics, new PxTriangleMeshGeometry(mesh)) });
	}

	Shape* PhysicsUtility::ShapeFromConvexTriangles(vector<vec3> vertices, Physics* physics)
	{
		PxConvexMeshDesc convexMeshDesc;
		convexMeshDesc.points.count = vertices.size();
		convexMeshDesc.points.stride = sizeof(vec3);
		convexMeshDesc.points.data = vertices.data();
		convexMeshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

		PxConvexMesh* convexMesh = NULL;
		PxDefaultMemoryOutputStream buf;
		if (physics->GetCooking()->cookConvexMesh(convexMeshDesc, buf))
		{
			PxDefaultMemoryInputData stream = PxDefaultMemoryInputData(buf.getData(), buf.getSize());
			convexMesh = physics->GetPhysics()->createConvexMesh(stream);
		}

		return new Shape(physics, new PxConvexMeshGeometry(convexMesh));
	}
}