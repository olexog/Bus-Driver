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

	string GetDirectory(string fileName)
	{
		size_t index = fileName.find_last_of("/\\");
		return fileName.substr(0, index + 1);
	}

	std::string TrimStart(std::string s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	std::string TrimEnd(std::string s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	std::string Trim(std::string s)
	{
		return TrimStart(TrimEnd(s));
	}

	vector<string> Split(string s, string delimiter)
	{
		vector<string> result;

		while (true)
		{
			int position = static_cast<int>(s.find(delimiter));

			if (position == string::npos)
			{
				result.push_back(s);
				return result;
			}

			result.push_back(s.substr(0, position));

			s = s.substr(position + delimiter.length(), s.length() - position - delimiter.length());
		}

		return result;
	}

	void GenerateSplitTest(string fileName)
	{
		ofstream file = ofstream(fileName);

		char chars[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
		const int count = sizeof(chars) / sizeof(char);
		const int wordLength = 50;
		const int separatorLength = 1;

		srand(time(nullptr));

		for (int i = 0; i < 10000; i++)
		{
			char word[wordLength];
			for (int j = 0; j < wordLength; j++)
			{
				word[j] = chars[rand() % count];
			}

			char separator[separatorLength];
			for (int j = 0; j < separatorLength; j++)
			{
				separator[j] = chars[rand() % count];
			}

			for (int j = 0; j < 5; j++)
			{
				int pos = rand() % (wordLength - separatorLength + 1);
				for (int k = 0; k < separatorLength; k++)
				{
					word[pos + k] = separator[k];
				}
			}

			string line = (string(word) + " " + string(separator) + "\n");
			file.write(line.c_str(), line.length());
		}

		file.close();
	}

	void SplitTest(string fileName)
	{

	}

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

	Model* ModelReader::Read(string fileName)
	{
		GenerateSplitTest("Split1.test");

		std::cerr << fileName << std::endl;

		ifstream file = ifstream(fileName);

		vector<vec3> vertices = vector<vec3>();
		vector<vec3> normals = vector<vec3>();
		vector<vec2> texCoords = vector<vec2>();

		vector<vec3> colouredVertices = vector<vec3>();
		vector<vec3> colouredNormals = vector<vec3>();
		vector<vec3> ambientColours = vector<vec3>();
		vector<vec3> diffuseColours = vector<vec3>();
		vector<vec3> specularColours = vector<vec3>();

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

				sscanf_s(line.c_str(), "%*255s %f %f %f", x, y, z);

				vertices.push_back(vec3(*x, *y, *z));
			}
			else if (strcmp(command, "vn") == 0)
			{
				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf_s(line.c_str(), "%*255s %f %f %f", x, y, z);

				normals.push_back(vec3(*x, *y, *z));
			}
			else if (strcmp(command, "vt") == 0)
			{
				float* x = new float();
				float* y = new float();

				sscanf_s(line.c_str(), "%*255s %f %f", x, y);

				texCoords.push_back(vec2(*x, *y));
			}
			else if (strcmp(command, "f") == 0)
			{
				string argumentString = line.substr(line.find(" ") + 1);

				vector<string> arguments = Split(argumentString, " ");

				vector<string> arguments1 = Split(arguments[0], "/");
				vector<string> arguments2 = Split(arguments[1], "/");
				vector<string> arguments3 = Split(arguments[2], "/");

				int vertex1 = atoi(arguments1[0].c_str());
				int vertex2 = atoi(arguments2[0].c_str());
				int vertex3 = atoi(arguments3[0].c_str());

				int normal1 = atoi(arguments1[2].c_str());
				int normal2 = atoi(arguments2[2].c_str());
				int normal3 = atoi(arguments3[2].c_str());

				colouredVertices.push_back(vertices[vertex1 - 1]);
				colouredVertices.push_back(vertices[vertex2 - 1]);
				colouredVertices.push_back(vertices[vertex3 - 1]);

				colouredNormals.push_back(normals[normal1 - 1]);
				colouredNormals.push_back(normals[normal2 - 1]);
				colouredNormals.push_back(normals[normal3 - 1]);

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
			else if (strcmp(command, "mtllib") == 0)
			{
				char materialLibraryRelativeFileName[256];

				sscanf_s(line.c_str(), "%*255s %255s", materialLibraryRelativeFileName, 256);

				string materialLibraryFileName = GetDirectory(fileName) + materialLibraryRelativeFileName;

				materials = ReadMaterialLibrary(materialLibraryFileName);
			}
			else if (strcmp(command, "usemtl") == 0)
			{
				char actualMaterialName[256];

				sscanf_s(line.c_str(), "%*255s %255s", actualMaterialName, 256);

				actualMaterial = materials[actualMaterialName];
			}
		}

		file.close();

		return new Model(new ColouredVertexArray(colouredVertices, colouredNormals, ambientColours, diffuseColours, specularColours));
	}
}
