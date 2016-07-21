#include "ModelReader.h"

namespace GraphicsLibrary
{
	struct Material
	{
	public:
		vec3 ambientColour;
		vec3 diffuseColour;
		vec3 specularColour;
	};

	Model* ModelReader::Read(string fileName)
	{
		ifstream file = ifstream(fileName);

		vector<vec3> vertices = vector<vec3>();
		vector<vec3> normals = vector<vec3>();
		vector<vec2> texCoords = vector<vec2>();

		vector<vec3> colouredVertices = vector<vec3>();

		string line;
		char command[256];

		map<string, Material> materials;
		Material actualMaterial;

		while (getline(file, line))
		{
			sscanf_s(line.c_str(), "%255s", command, 256);

			if (strcmp(command, "v") == 0)
			{
				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf_s(line.c_str(), "%*255s %f %f %f", command, 256, x, y, z);

				vertices.push_back(vec3(*x, *y, *z));
			}
			else if (strcmp(command, "vn") == 0)
			{
				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf_s(line.c_str(), "%*255s %f %f %f", command, 256, x, y, z);

				normals.push_back(vec3(*x, *y, *z));
			}
			else if (strcmp(command, "vt") == 0)
			{
				float* x = new float();
				float* y = new float();

				sscanf_s(line.c_str(), "%*255s %f %f", command, 256, x, y);

				texCoords.push_back(vec2(*x, *y));
			}
			else if (strcmp(command, "f") == 0)
			{
				int* vertex1 = new int();
				int* vertex2 = new int();
				int* vertex3 = new int();

				sscanf_s(line.c_str(), "%*255s %d%*s %d%*s %d%*s", command, 256, vertex1, vertex2, vertex3);

				colouredVertices.push_back(vertices[*vertex1 - 1]);
				colouredVertices.push_back(vertices[*vertex2 - 1]);
				colouredVertices.push_back(vertices[*vertex3 - 1]);
			}
			else if (strcmp(command, "mtllib") == 0)
			{
				char materialLibraryFileName[256];

				sscanf_s(line.c_str(), "%*255s %255s", materialLibraryFileName, 256);

				materials = ReadMaterialLibrary(materialLibraryFileName);
			}
			else if (strcmp(command, "usemtl") == 0)
			{
				char actualMaterialName[256];

				sscanf_s(line.c_str(), "%*255s %255s", actualMaterialName, 256);

				actualMaterial = materials[actualMaterialName];
			}
		}

		return new Model(new ColouredVertexArray(colouredVertices, colouredVertices));
	}

	map<string, Material> ModelReader::ReadMaterialLibrary(string fileName)
	{
		ifstream file = ifstream(fileName);

		string line;
		char command[256];
		string actualMaterialName = "";

		map<string, Material> materials = map<string, Material>();

		while (getline(file, line))
		{
			sscanf_s(line.c_str(), "%255s", command, 256);

			if (strcmp(command, "newmtl") == 0)
			{
				char materialName[256];

				sscanf_s(line.c_str(), "%*255s %255s", materialName, 256);

				actualMaterialName = materialName;
				continue;
			}

			if (strcmp(actualMaterialName.c_str(), "") == 0)
			{
				continue;
			}

			if (strcmp(command, "Ka") == 0)
			{
				float* red = new float();
				float* green = new float();
				float* blue = new float();

				sscanf_s(line.c_str(), "%*255s %f %f %f", red, green, blue);

				materials[actualMaterialName].ambientColour = vec3(red, green, blue);
			}
			else if (strcmp(command, "Kd") == 0)
			{
				float* red = new float();
				float* green = new float();
				float* blue = new float();

				sscanf_s(line.c_str(), "%*255s %f %f %f", red, green, blue);

				materials[actualMaterialName].diffuseColour = vec3(red, green, blue);
			}
			else if (strcmp(command, "Ks") == 0)
			{
				float* red = new float();
				float* green = new float();
				float* blue = new float();

				sscanf_s(line.c_str(), "%*255s %f %f %f", red, green, blue);

				materials[actualMaterialName].specularColour = vec3(red, green, blue);
			}
		}

		return materials;
	}
}
