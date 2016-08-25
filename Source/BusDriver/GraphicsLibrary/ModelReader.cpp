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

	map<string, Material> ReadMaterialLibrary(string fileName)
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

				materials[actualMaterialName].ambientColour = vec3(*red, *green, *blue);
			}
			else if (strcmp(command, "Kd") == 0)
			{
				float* red = new float();
				float* green = new float();
				float* blue = new float();

				sscanf_s(line.c_str(), "%*255s %f %f %f", red, green, blue);

				materials[actualMaterialName].diffuseColour = vec3(*red, *green, *blue);
			}
			else if (strcmp(command, "Ks") == 0)
			{
				float* red = new float();
				float* green = new float();
				float* blue = new float();

				sscanf_s(line.c_str(), "%*255s %f %f %f", red, green, blue);

				materials[actualMaterialName].specularColour = vec3(*red, *green, *blue);
			}
		}

		file.close();

		return materials;
	}

	int* ParseFace(const char* &token)
	{
		int* result = new int[3];

		result[0] = atoi(token);
		token += strcspn(token, "/ \t\r\n");

		// a vertex index singleton
		if (token[0] != '/')
		{
			return result;
		}

		// skip the / sign
		token++;

		// texture coordinate skipped
		if (token[0] == '/')
		{
			token++;

			result[2] = atoi(token);
			token += strcspn(token, "/ \t\r\n");

			return result;
		}

		result[1] = atoi(token);
		token += strcspn(token, "/ \t\r\n");

		if (token[0] != '/')
		{
			return result;
		}

		token++;

		result[2] = atoi(token);
		token += strcspn(token, "/ \t\r\n");

		return result;
	}

	Model* ModelReader::Read(string fileName)
	{
		return ModelReader::Read(fileName, vector<vec3>());
	}

	Model* ModelReader::Read(string fileName, vector<vec3> &outVertices)
	{
		cout << "Loading " << fileName << endl;

		ifstream file = ifstream(fileName);

		vector<vec3> vertices = vector<vec3>();
		vector<vec3> normals = vector<vec3>();
		vector<vec2> texCoords = vector<vec2>();

		vector<vec3> colouredVertices = vector<vec3>();
		vector<vec3> colouredNormals = vector<vec3>();
		vector<vec2> colouredTexCoords = vector<vec2>();
		vector<vec3> ambientColours = vector<vec3>();
		vector<vec3> diffuseColours = vector<vec3>();
		vector<vec3> specularColours = vector<vec3>();

		string line;

		map<string, Material> materials;
		Material actualMaterial;

		while (getline(file, line))
		{
			const char* token = line.c_str();
			token += strspn(token, " \t\r\n");

			if (token[0] == '\0')
				continue;

			if (token[0] == '#')
				continue;

			if (token[0] == 'v' && token[1] == ' ')
			{
				token += 1;
				token += strspn(token, " \t");

				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf_s(token, "%f %f %f", x, y, z);

				vertices.push_back(vec3(*x, *y, *z));
			}
			else if (token[0] == 'v' && token[1] == 'n' && token[2] == ' ')
			{
				token += 2;
				token += strspn(token, " \t");

				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf_s(token, "%f %f %f", x, y, z);

				normals.push_back(vec3(*x, *y, *z));
			}
			else if (token[0] == 'v' && token[1] == 't' && token[2] == ' ')
			{
				token += 2;
				token += strspn(token, " \t");

				float* x = new float();
				float* y = new float();

				sscanf_s(token, "%f %f", x, y);

				texCoords.push_back(vec2(*x, -*y));
			}
			else if (token[0] == 'f' && token[1] == ' ')
			{
				token += 1;
				token += strspn(token, " \t");

				int* face1 = ParseFace(token);

				token += strspn(token, " \t");

				int* face2 = ParseFace(token);

				token += strspn(token, " \t");

				int* face3 = ParseFace(token);

				int vertex1 = face1[0];
				int vertex2 = face2[0];
				int vertex3 = face3[0];

				int texCoord1 = face1[1];
				int texCoord2 = face2[1];
				int texCoord3 = face3[1];

				int normal1 = face1[2];
				int normal2 = face2[2];
				int normal3 = face3[2];

				colouredVertices.push_back(vertices[vertex1 - 1]);
				colouredVertices.push_back(vertices[vertex2 - 1]);
				colouredVertices.push_back(vertices[vertex3 - 1]);

				colouredNormals.push_back(normals[normal1 - 1]);
				colouredNormals.push_back(normals[normal2 - 1]);
				colouredNormals.push_back(normals[normal3 - 1]);

				colouredTexCoords.push_back(texCoords[texCoord1 - 1]);
				colouredTexCoords.push_back(texCoords[texCoord2 - 1]);
				colouredTexCoords.push_back(texCoords[texCoord3 - 1]);

				ambientColours.push_back(actualMaterial.ambientColour);
				ambientColours.push_back(actualMaterial.ambientColour);
				ambientColours.push_back(actualMaterial.ambientColour);

				diffuseColours.push_back(actualMaterial.diffuseColour);
				diffuseColours.push_back(actualMaterial.diffuseColour);
				diffuseColours.push_back(actualMaterial.diffuseColour);

				specularColours.push_back(actualMaterial.specularColour);
				specularColours.push_back(actualMaterial.specularColour);
				specularColours.push_back(actualMaterial.specularColour);
			}
			else if (strncmp(token, "mtllib", 6) == 0 && token[6] == ' ')
			{
				token += 6;
				token += strspn(token, " \t");
				const char* materialLibraryRelativeFileName = token;

				string materialLibraryFileName = Utility::GetDirectory(fileName) + materialLibraryRelativeFileName;

				materials = ReadMaterialLibrary(materialLibraryFileName);
			}
			else if (strncmp(token, "usemtl", 6) == 0 && token[6] == ' ')
			{
				token += 6;
				token += strspn(token, " \t");
				const char* actualMaterialName = token;

				actualMaterial = materials[actualMaterialName];
			}
		}

		file.close();

		outVertices = colouredVertices;

		return new Model({ new VertexArray(colouredVertices, colouredNormals, colouredTexCoords, NULL) });
	}
}
