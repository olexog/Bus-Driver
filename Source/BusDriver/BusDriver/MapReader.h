#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "Map.h"
#include "ThingyReader.h"
#include "Utility.h"

using namespace std;

class MapReader
{
public:
	static Map Read(string fileName);
};

