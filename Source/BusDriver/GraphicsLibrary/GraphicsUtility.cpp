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
}