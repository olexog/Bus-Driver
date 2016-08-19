#include "PhysicsUtility.h"

namespace PhysicsLibrary
{
	Body* PhysicsUtility::BodyFromTriangles(vector<vec3> vertices, Physics* physics)
	{
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = vertices.size();
		meshDesc.points.stride = sizeof(vec3);
		meshDesc.points.data = vertices.data();

		PxU32* indices = new PxU32[vertices.size()];
		for (int i = 0; i < vertices.size(); i++)
		{
			indices[i] = i;
		}

		meshDesc.triangles.count = vertices.size();
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = indices;

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

	Body* PhysicsUtility::BodyFromConvexTriangles(vector<vec3> vertices, Physics* physics)
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

		return new Body({ new Shape(physics, new PxConvexMeshGeometry(convexMesh)) });
	}
}