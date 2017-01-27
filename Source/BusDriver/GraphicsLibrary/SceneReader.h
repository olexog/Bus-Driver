#pragma once

#include <string>
#include <fstream>
#include <map>

#include "Scene.h"
#include "ModelReader.h"
#include "GraphicsUtility.h"

using namespace std;

namespace GraphicsLibrary
{
	class SceneReader
	{
	public:
		static Scene* Read(string fileName);
	};
}

