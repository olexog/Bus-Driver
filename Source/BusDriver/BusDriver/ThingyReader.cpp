#include "ThingyReader.h"

///<summary>Represents the whitespace characters.</summary>
const char* WHITESPACES = " \t\r\n";

Thingy* ThingyReader::Read(Physics* physics, string fileName)
{
	Model* model = NULL;
	Body* body = NULL;

	// open file stream
	ifstream file = ifstream(fileName);

	// initialize variables necessary to read
	int lineIndex = 0;
	string line;
	char command[256];

	// iterate through the file
	while (getline(file, line))
	{
		lineIndex++;

		// initialize a shiftable pointer to the line's C string
		const char* token = line.c_str();
		// skip whitespaces
		token += strspn(token, WHITESPACES);

		// skip empty lines and comments
		if (token[0] == '\0' || token[0] == '#')
			continue;

		// read command
		sscanf_s(token, "%255s", command, 256);
		token += strlen(command);
		// skip whitespaces
		token += strspn(token, WHITESPACES);

		// if command is "load"
		if (strcmp(command, "load") == 0)
		{
			// if the next character is not a quotation mark
			if (token[0] != '\"')
			{
				cerr << "The first parameter of load must be a string." << endl;
				continue;
			}

			// skip the quotation mark
			token += 1;

			// get the file name up to the next quotation mark
			int fileNameLength = strcspn(token, "\"");
			string relativeFileName = string(token, fileNameLength);

			// skip the file name
			token += fileNameLength;
			// skip the quotation mark
			token += 1;
			// skip whitespaces
			token += strspn(token, WHITESPACES);
			// skip to
			token += 2;
			// skip whitespaces
			token += strspn(token, WHITESPACES);

			// get the variable name
			int variableNameLength = strcspn(token, WHITESPACES);
			string variableName = string(token, variableNameLength);

			if (variableName == "graphicsThingy")
			{
				model = ModelReader::Read(Utility::GetDirectory(fileName) + relativeFileName);
			}
			else if (variableName == "physicsThingy")
			{
				body = BodyReader::Read(physics, Utility::GetDirectory(fileName) + relativeFileName);
			}
			else if (variableName == "graphysicsThingy")
			{
				vector<vec3> vertices;
				model = ModelReader::Read(Utility::GetDirectory(fileName) + relativeFileName, vertices);
				body = PhysicsUtility::BodyFromTriangles(vertices, physics);
			}
			// if variable is unknown
			else
			{
				cout << "Unknown variable \"" << variableName << "\" at line " << lineIndex << " while reading thingy file " << fileName << "." << endl;
			}
		}
		// if command is unknown
		else
		{
			cout << "Unknown command \"" << command << "\" at line " << lineIndex << " while reading thingy file " << fileName << "." << endl;
		}
	}

	return new Thingy(model, body);
}
