#include "ModelReader.h"

namespace GraphicsLibrary
{
	ModelReader::ModelReader()
	{
	}

	ModelReader::~ModelReader()
	{
	}

	void ModelReader::Read(string fileName, Model* &model)
	{
		/*ifstream file = ifstream(fileName);

		string line;

		vector<PxVec3>* vertices = new vector<PxVec3>();
		vector<PxVec3>* normals = new vector<PxVec3>();
		vector<PxVec2>* texCoords = new vector<PxVec2>();

		while (getline(file, line))
		{
			char command[256];

			sscanf(line.c_str(), "%s", command);

			if (strcmp(command, "v") == 0)
			{
				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf(line.c_str(), "%s %f %f %f", command, x, y, z);

				vertices->push_back(PxVec3(*x, *y, *z));
			}
			else if (strcmp(command, "vn") == 0)
			{
				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf(line.c_str(), "%s %f %f %f", command, x, y, z);

				normals->push_back(PxVec3(*x, *y, *z));
			}
			else if (strcmp(command, "vt") == 0)
			{
				float* x = new float();
				float* y = new float();

				sscanf(line.c_str(), "%s %f %f", command, x, y);

				texCoords->push_back(PxVec2(*x, *y));
			}
			else if (strcmp(command, "f") == 0)
			{

			}
		}*/
	}
}
