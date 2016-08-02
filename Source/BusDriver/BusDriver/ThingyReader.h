#pragma once

#include <string>


#include "Utility.h"
#include "Thingy.h"
#include "ModelReader.h"

using namespace std;

class ThingyReader
{
public:
	static Thingy* Read(string fileName);
};

