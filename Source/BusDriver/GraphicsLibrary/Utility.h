#pragma once

#include <string>
#include <vector>
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;

namespace GraphicsLibrary
{
	class Utility
	{
	public:
		static string GetDirectory(string fileName);

		static string TrimStart(string s);

		static string TrimEnd(string s);

		static string Trim(string s);

		static vector<string> Split(string s, string delimiter);
	};
}