#include "ModelReader.h"

namespace GraphicsLibrary
{
	void ModelReader::Read(string fileName, Model* &model)
	{
		ifstream file = ifstream(fileName);

		string line;

		vector<vec3>* vertices = new vector<vec3>();
		vector<vec3>* normals = new vector<vec3>();
		vector<vec2>* texCoords = new vector<vec2>();

		while (getline(file, line))
		{
			char command[256];

			sscanf_s(line.c_str(), "%s", command, 256);

			if (strcmp(command, "v") == 0)
			{
				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf_s(line.c_str(), "%s %f %f %f", command, 256, x, y, z);

				vertices->push_back(vec3(*x, *y, *z));
			}
			else if (strcmp(command, "vn") == 0)
			{
				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf_s(line.c_str(), "%s %f %f %f", command, 256, x, y, z);

				normals->push_back(vec3(*x, *y, *z));
			}
			else if (strcmp(command, "vt") == 0)
			{
				float* x = new float();
				float* y = new float();

				sscanf_s(line.c_str(), "%s %f %f", command, 256, x, y);

				texCoords->push_back(vec2(*x, *y));
			}
			else if (strcmp(command, "f") == 0)
			{

			}
		}
	}
}
