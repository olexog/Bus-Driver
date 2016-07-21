#include "ModelReader.h"

namespace GraphicsLibrary
{
	void ModelReader::Read(string fileName, Model* &model)
	{
		ifstream file = ifstream(fileName);

		string line;

		vector<vec3> vertices = vector<vec3>();
		vector<vec3> normals = vector<vec3>();
		vector<vec2> texCoords = vector<vec2>();

		vector<vec3> colouredVertices = vector<vec3>();

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

				vertices.push_back(vec3(*x, *y, *z));
			}
			else if (strcmp(command, "vn") == 0)
			{
				float* x = new float();
				float* y = new float();
				float* z = new float();

				sscanf_s(line.c_str(), "%s %f %f %f", command, 256, x, y, z);

				normals.push_back(vec3(*x, *y, *z));
			}
			else if (strcmp(command, "vt") == 0)
			{
				float* x = new float();
				float* y = new float();

				sscanf_s(line.c_str(), "%s %f %f", command, 256, x, y);

				texCoords.push_back(vec2(*x, *y));
			}
			else if (strcmp(command, "f") == 0)
			{
				int* vertex1 = new int();
				int* vertex2 = new int();
				int* vertex3 = new int();

				sscanf_s(line.c_str(), "%s %d%*s %d%*s %d%*s", command, 256, vertex1, vertex2, vertex3);

				colouredVertices.push_back(vertices[*vertex1 - 1]);
				colouredVertices.push_back(vertices[*vertex2 - 1]);
				colouredVertices.push_back(vertices[*vertex3 - 1]);
			}
		}

		model = new Model(new ColouredVertexArray(colouredVertices, colouredVertices));
	}
}
