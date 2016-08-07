#include "MapReader.h"

Map MapReader::Read(Physics* physics, string fileName)
{
	// the thingies which the map consists of
	map<string, Thingy*> thingies;

	vector<PositionedThingy*> positionedThingies;

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
		token += strspn(token, Utility::WHITESPACES);

		// skip empty lines and comments
		if (token[0] == '\0' || token[0] == '#')
			continue;

		// read command
		sscanf_s(token, "%255s", command, 256);
		token += strlen(command);
		// skip whitespaces
		token += strspn(token, Utility::WHITESPACES);

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
			token += strspn(token, Utility::WHITESPACES);
			// skip to
			token += 2;
			// skip whitespaces
			token += strspn(token, Utility::WHITESPACES);
			// skip thingies
			token += 8;

			// check the indexer
			if (token[0] != '[' || token[1] != '\"')
			{
				cerr << "Indexer cannot be found at load command." << endl;
			}

			// skip the indexer and the quotation mark
			token += 2;

			// get the key up to the next quotation mark
			int keyLength = strcspn(token, "\"");
			string key = string(token, keyLength);

			// load the thingy
			thingies[key] = ThingyReader::Read(physics, Utility::GetDirectory(fileName) + relativeFileName);
		}
		// if command is "place"
		else if (strcmp(command, "place") == 0)
		{
			// skip whitespaces
			token += strspn(token, Utility::WHITESPACES);
			// skip thingies
			token += 8;

			// check the indexer
			if (token[0] != '[' || token[1] != '\"')
			{
				cerr << "Indexer cannot be found at load command." << endl;
			}

			// skip the indexer and the quotation mark
			token += 2;

			// get the key up to the next quotation mark
			int length = strcspn(token, "\"");
			string key = string(token, length);

			// skip the file name
			token += length;
			// skip the closing bracket and the quotation mark
			token += 2;
			// skip whitespaces
			token += strspn(token, Utility::WHITESPACES);
			// skip "to"
			token += 2;
			// skip whitespaces
			token += strspn(token, Utility::WHITESPACES);

			// check if it is a vector
			if (token[0] != '(')
			{
				cerr << "Vector was expected as second argument in command place.";
				continue;
			}

			// read position
			float* x = new float();
			float* y = new float();
			float* z = new float();
			sscanf_s(token, "(%f; %f; %f)", x, y, z);
			// set position
			vec3* position = new vec3(*x, *y, *z);

			positionedThingies.push_back(new PositionedThingy(thingies[key], position, new quat()));
		}
		// if command is unknown
		else
		{
			cout << "Unknown command \"" << command << "\" at line " << lineIndex << " while reading map file " << fileName << "." << endl;
		}
	}

	// collect thingies
	vector<Thingy*> thingyVector;
	for (map<string, Thingy*>::iterator iterator = thingies.begin(); iterator != thingies.end(); ++iterator)
	{
		thingyVector.push_back(iterator->second);
	}

	// return the read positioned geometries
	return Map(thingyVector, positionedThingies);
}