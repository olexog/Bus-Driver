#include "ModelReader.h"

namespace GraphicsLibrary
{
	struct Material
	{
	public:
		vec3 ambientColour;
		vec3 diffuseColour;
		vec3 specularColour;
		string* texturePath;
	};

	struct Collection
	{
	public:
		vector<vec3> vertices;
		vector<vec3> normals;
		vector<vec2> texCoords;
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

		map<string, Collection> texturedCollections;
		map<string, Collection> colouredCollections;

		vector<vec3> vertices = vector<vec3>();
		vector<vec3> normals = vector<vec3>();
		vector<vec2> texCoords = vector<vec2>();

		string line;

		map<string, Material> materials;
		string actualMaterialName;

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

				vector<int> vertexIndices = { face1[0], face2[0], face3[0] };
				vector<int> texCoordIndices = { face1[1], face2[1], face3[1] };
				vector<int> normalIndices = { face1[2], face2[2], face3[2] };

				if (materials[actualMaterialName].texturePath == NULL)
				{
					for (int i = 0; i < 3; i++)
					{
						colouredCollections[actualMaterialName].vertices.push_back(vertices[vertexIndices[i] - 1]);
						colouredCollections[actualMaterialName].texCoords.push_back(texCoords[texCoordIndices[i] - 1]);
						colouredCollections[actualMaterialName].normals.push_back(normals[normalIndices[i] - 1]);
					}
				}
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
				actualMaterialName = string(token);
			}
		}

		file.close();

		vector<VertexArray*> vertexArrays;

		outVertices = vector<vec3>();
		
		int colourCount = colouredCollections.size();
		vector<char> colourBytes = vector<char>(colourCount);

		Texture* colourTexture = new Texture();
		colourTexture->Bind();
		colourTexture->LoadData(2, 1, colourBytes.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		Texture::Unbind();

		for (std::pair<string, Collection> collection : colouredCollections)
		{
			outVertices.reserve(outVertices.size() + collection.second.vertices.size());
			outVertices.insert(outVertices.end(), collection.second.vertices.begin(), collection.second.vertices.end());

			vec3 colour = materials[collection.first].diffuseColour;
			colourBytes.push_back(static_cast<char>(colour.r * 256));
			colourBytes.push_back(static_cast<char>(colour.g * 256));
			colourBytes.push_back(static_cast<char>(colour.b * 256));

			vector<vec2> texCoords = vector<vec2>(collection.second.vertices.size(), vec2((colourBytes.size() / 3) - 1, 0));

			vertexArrays.push_back(new VertexArray(collection.second.vertices, collection.second.normals, texCoords, colourTexture));
		}

		return new Model(vertexArrays);
	}
}
