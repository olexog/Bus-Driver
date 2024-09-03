#include "GraphicsUtility.h"

namespace GraphicsLibrary
{
	string GraphicsUtility::GetDirectory(string fileName)
	{
		size_t index = fileName.find_last_of("/\\");
		return fileName.substr(0, index + 1);
	}

	string GraphicsUtility::TrimStart(string s)
	{
		s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
		return s;
	}

	string GraphicsUtility::TrimEnd(string s)
	{
		s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
		return s;
	}

	string GraphicsUtility::Trim(string s)
	{
		return TrimStart(TrimEnd(s));
	}

	vector<string> GraphicsUtility::Split(string s, char delimiter)
	{
		stringstream input(s);
		vector<string> result = vector<string>();

		string word;
		while (getline(input, word, delimiter))
		{
			result.push_back(word);
		}

		return result;
	}

	vector<string> GraphicsUtility::Split(string s, string delimiter)
	{
		const size_t sLength = s.length();
		const size_t delimiterLength = delimiter.length();

		vector<string> result = vector<string>();
		int stringStart = 0;
		int delimiterPos = 0;

		for (int pos = 0; pos < sLength; pos++)
		{
			if (s[pos] == delimiter[delimiterPos])
			{
				delimiterPos++;

				if (delimiterPos == delimiterLength)
				{
					string substring = s.substr(stringStart, pos - delimiterLength - stringStart + 1);
					result.push_back(substring);

					stringStart = pos + 1;
					delimiterPos = 0;
				}
			}
			else
			{
				delimiterPos = 0;
			}
		}

		result.push_back(s.substr(stringStart, sLength - stringStart));

		return result;
	}

	vec3 GraphicsUtility::Transform(vec3 vector, mat4 matrix)
	{
		vec4 result = matrix * vec4(vector, 1.0f);
		return vec3(result.x / result.w, result.y / result.w, result.z / result.w);
	}

	string GraphicsUtility::ToString(float number, int precision)
	{
		stringstream stream;
		stream << fixed << setprecision(2) << number;
		return stream.str();
	}

	string GraphicsUtility::ToString(quat quaternion, int precision)
	{
		string x = GraphicsUtility::ToString(quaternion.x, precision);
		string y = GraphicsUtility::ToString(quaternion.y, precision);
		string z = GraphicsUtility::ToString(quaternion.z, precision);
		string w = GraphicsUtility::ToString(quaternion.w, precision);
		return "(" + x + "," + y + "," + z + "," + w + ")";
	}

	Texture* GraphicsUtility::LoadTexture(string fileName)
	{
		int width, height;
		unsigned char* data = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		Texture* result = new Texture();
		result->Bind();
		result->LoadData(width, height, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		Texture::Unbind();

		SOIL_free_image_data(data);

		return result;
	}

	vector<string> SlowSplit(string s, string delimiter)
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

	void SplitEqualityTest(string fileName)
	{
		// Read test file
		ifstream file = ifstream(fileName);

		struct Entry
		{
		public:
			string s;
			string c;
		};

		vector<Entry> entries = vector<Entry>();

		string line;
		while (getline(file, line))
		{
			char s[256];
			char* c = new char();
			sscanf_s(line.c_str(), "%255s", s, 256);
			sscanf_s(line.c_str(), "%*255s %c", c, 1);

			Entry entry;
			entry.s = s;
			entry.c = *c;
			entries.push_back(entry);
		}

		file.close();

		// Equality test
		cout << "Split equality test started..." << endl;

		for (Entry entry : entries)
		{
			vector<string> result = GraphicsUtility::Split(entry.s, entry.c);
			vector<string> slowResult = SlowSplit(entry.s, entry.c);

			if (result.size() != slowResult.size())
			{
				cerr << "Result sizes are not equal!" << endl;
				cerr << "Input string: " << entry.s << endl;

				for (int i = 0; i < result.size(); i++)
				{
					cerr << "result[" << i << "] = " << result[i] << endl;
				}

				for (int i = 0; i < slowResult.size(); i++)
				{
					cerr << "slowResult[" << i << "] = " << slowResult[i] << endl;
				}

				return;
			}

			for (int i = 0; i < result.size(); i++)
			{
				if (strcmp(result[i].c_str(), slowResult[i].c_str()) != 0)
				{
					cerr << "Results are not equal!" << endl;
					cerr << "Input string: " << entry.s << endl;

					for (int j = 0; j < result.size(); j++)
					{
						if (i == j)
						{
							cerr << "DIFFERENT: " << endl;
						}
						cerr << "result[" << i << "] = " << result[i] << endl;
						cerr << "slowResult[" << i << "] = " << slowResult[i] << endl;
					}

					return;
				}
			}
		}

		cout << "Split equality test passed." << endl;
	}

	void SplitPerformanceTest(string fileName)
	{
		// Read test file
		ifstream file = ifstream(fileName);

		struct Entry
		{
		public:
			string s;
			string c;
		};

		vector<Entry> entries = vector<Entry>();

		string line;
		while (getline(file, line))
		{
			char s[256];
			char* c = new char();
			sscanf_s(line.c_str(), "%255s", s, 256);
			sscanf_s(line.c_str(), "%*255s %c", c, 1);

			Entry entry;
			entry.s = s;
			entry.c = *c;
			entries.push_back(entry);
		}

		file.close();

		// Performance test
		cout << "Starting split performance test..." << endl;

		clock_t startTime = clock();

		for (Entry entry : entries)
		{
			vector<string> result = GraphicsUtility::Split(entry.s, entry.c);
		}

		clock_t endTime = clock();

		cout << "Finished. Elapsed time: " << 1000 * (endTime - startTime) / CLOCKS_PER_SEC << " ms." << endl;
	}
}