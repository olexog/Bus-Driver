#include "Actor.h"

PhysicsLibrary::Actor::Actor(Physics* physics, Body* body, vec3* position, quat* orientation)
{
	this->body = body;
	this->position = position;
	this->orientation = orientation;

	PxVec3 pxPosition = PxVec3(position->x, position->y, position->z);
	PxQuat pxOrientation = PxQuat(orientation->x, orientation->y, orientation->z, orientation->w);

	this->actor = physics->GetPhysics()->createRigidStatic(PxTransform(pxPosition, pxOrientation));
	this->body->AddToActor(physics, this->actor);
}

void PhysicsLibrary::Actor::AddToScene(PxScene* scene)
{
	scene->addActor(*this->actor);
}

vec3 PhysicsLibrary::Actor::GetPosition()
{
	return *this->position;
}

vector<vec3> PhysicsLibrary::Actor::GetPoints()
{
	vector<vec3> result;

	PxU32 shapeCount = this->actor->getNbShapes();
	PxShape** shapes = new PxShape*[shapeCount];
	this->actor->getShapes(shapes, shapeCount);

	for (int i = 0; i < shapeCount; i++)
	{
		PxTransform pose = shapes[i]->getLocalPose();
		vec3 translation = vec3(pose.p.x, pose.p.y, pose.p.z);

		PxGeometryHolder geometry = shapes[i]->getGeometry();

		switch (geometry.getType())
		{
		case PxGeometryType::eBOX:
		{
			vec3 size = vec3(geometry.box().halfExtents.x, geometry.box().halfExtents.y, geometry.box().halfExtents.z);

			static int faces[6][4] =
			{
				{ 0, 1, 2, 3 },
				{ 3, 2, 6, 7 },
				{ 7, 6, 5, 4 },
				{ 4, 5, 1, 0 },
				{ 5, 6, 2, 1 },
				{ 7, 4, 0, 3 }
			};
			float v[8][3];

			v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
			v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
			v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
			v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
			v[0][2] = v[3][2] = v[4][2] = v[7][2] = -1;
			v[1][2] = v[2][2] = v[5][2] = v[6][2] = 1;

			for (int i = 5; i >= 0; i--) {
				result.push_back(vec3(v[faces[i][0]][0], v[faces[i][0]][1], v[faces[i][0]][2]) * size + translation);
				result.push_back(vec3(v[faces[i][1]][0], v[faces[i][1]][1], v[faces[i][1]][2]) * size + translation);
				result.push_back(vec3(v[faces[i][2]][0], v[faces[i][2]][1], v[faces[i][2]][2]) * size + translation);

				result.push_back(vec3(v[faces[i][0]][0], v[faces[i][0]][1], v[faces[i][0]][2]) * size + translation);
				result.push_back(vec3(v[faces[i][2]][0], v[faces[i][2]][1], v[faces[i][2]][2]) * size + translation);
				result.push_back(vec3(v[faces[i][3]][0], v[faces[i][3]][1], v[faces[i][3]][2]) * size + translation);
			}
		} break;
		case PxGeometryType::eCONVEXMESH:
		{
			//Compute triangles for each polygon.
			const PxVec3 scale = geometry.convexMesh().scale.scale;
			PxConvexMesh* mesh = geometry.convexMesh().convexMesh;
			const PxU32 nbPolys = mesh->getNbPolygons();
			const PxU8* polygons = mesh->getIndexBuffer();
			const PxVec3* verts = mesh->getVertices();
			PxU32 nbVerts = mesh->getNbVertices();
			PX_UNUSED(nbVerts);

			PxU32 numTotalTriangles = 0;
			for (PxU32 i = 0; i < nbPolys; i++)
			{
				PxHullPolygon data;
				mesh->getPolygonData(i, data);

				const PxU32 nbTris = data.mNbVerts - 2;
				const PxU8 vref0 = polygons[data.mIndexBase + 0];
				PX_ASSERT(vref0 < nbVerts);
				for (PxU32 j = 0; j < nbTris; j++)
				{
					const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
					const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

					result.push_back(vec3(verts[vref0].x, verts[vref0].y, verts[vref0].z) * vec3(scale.x, scale.y, scale.z) + translation);
					result.push_back(vec3(verts[vref1].x, verts[vref1].y, verts[vref1].z) * vec3(scale.x, scale.y, scale.z) + translation);
					result.push_back(vec3(verts[vref2].x, verts[vref2].y, verts[vref2].z) * vec3(scale.x, scale.y, scale.z) + translation);
				}
			}
		} break;
		}
	}
	return result;
}

PhysicsLibrary::Actor::~Actor()
{
}
