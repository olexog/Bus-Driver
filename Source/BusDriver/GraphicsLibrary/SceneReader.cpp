#include "SceneReader.h"

namespace GraphicsLibrary
{
	Scene* SceneReader::Read(string fileName)
	{
		ifstream file = ifstream(fileName);

		string line;
		char command[256];

		Scene* scene = new Scene();

		map<int, Model*> models = map<int, Model*>();

		int currentModelIndex = 0;

		// pass first line
		getline(file, line);

		while (getline(file, line))
		{
			sscanf_s(line.c_str(), "%255s", command, 256);

			if (strcmp(command, "addmodel") == 0)
			{
				char path[256];
				sscanf_s(line.c_str(), "%*s %255s", path, 256);

				models[currentModelIndex++] = ModelReader::Read(Utility::GetDirectory(fileName) + path);
			}
			else if (strcmp(command, "defineobject") == 0)
			{
				int* index = new int();
				float* x = new float();
				float* y = new float();
				float* z = new float();
				float* rotateX = new float();
				float* rotateY = new float();
				float* rotateZ = new float();

				sscanf_s(line.c_str(), "%*s %d, %f, %f, %f, %f, %f, %f", index, x, y, z, rotateX, rotateY, rotateZ);

				vec3 position = vec3(*x, *y, -(*z));
				quat rotation = quat(radians(vec3(*rotateX, *rotateY, *rotateZ)));
				scene->models.push_back(new PositionedModel(models[*index], position, rotation));
			}
		}

		return scene;
	}
}
