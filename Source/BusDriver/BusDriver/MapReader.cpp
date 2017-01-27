#include "MapReader.h"

Map MapReader::Read(Physics* physics, string fileName)
{
	// the thingies which the map consists of
	map<string, Thingy*> thingies;
	map<string, Thingy*> models;

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
		token += strspn(token, GraphicsUtility::WHITESPACES);

		// skip empty lines and comments
		if (token[0] == '\0' || token[0] == '#')
			continue;

		// read command
		sscanf_s(token, "%255s", command, 256);
		token += strlen(command);
		// skip whitespaces
		token += strspn(token, GraphicsUtility::WHITESPACES);

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
			token += strspn(token, GraphicsUtility::WHITESPACES);
			// skip to
			token += 2;
			// skip whitespaces
			token += strspn(token, GraphicsUtility::WHITESPACES);

			// parse variable name
			string variableName = string(token, strcspn(token, "["));
			token += variableName.length();

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
			if (variableName == "thingies")
			{
				thingies[key] = ThingyReader::Read(physics, GraphicsUtility::GetDirectory(fileName) + relativeFileName);
			}
			else if (variableName == "models")
			{
				models[key] = new Thingy(ModelReader::Read(GraphicsUtility::GetDirectory(fileName) + relativeFileName), NULL);
			}
			else
			{
				cout << "Unknown variable name \"" << variableName << "\" at line " << lineIndex << " while reading map file " << fileName << "." << endl;
			}
		}
		// if command is "place"
		else if (strcmp(command, "place") == 0)
		{
			// skip whitespaces
			token += strspn(token, GraphicsUtility::WHITESPACES);

			// parse variable name
			string variableName = string(token, strcspn(token, "["));
			token += variableName.length();

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
			token += strspn(token, GraphicsUtility::WHITESPACES);
			// skip "to"
			token += 2;
			// skip whitespaces
			token += strspn(token, GraphicsUtility::WHITESPACES);

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

			// initialize orientation
			quat* orientation = new quat(1, 0, 0, 0);

			// skip vector
			token += strcspn(token, ")");
			token += 1;

			// skip whitespaces
			token += strspn(token, GraphicsUtility::WHITESPACES);

			if (strcmp(token, "") != 0)
			{
				// skip "as"
				token += 2;
				// skip whitespaces
				token += strspn(token, GraphicsUtility::WHITESPACES);
				
				// read orientation
				float* x = new float();
				float* y = new float();
				float* z = new float();
				float* w = new float();
				sscanf_s(token, "(%f; %f; %f; %f)", x, y, z, w);
				// set orientation
				orientation = new quat(*w, *x, *y, *z);
			}

			if (variableName == "thingies")
			{
				positionedThingies.push_back(new PositionedThingy(thingies[key], position, orientation));
			}
			else if (variableName == "models")
			{
				positionedThingies.push_back(new PositionedThingy(models[key], position, orientation));
			}
			else
			{
				cout << "Unknown variable name \"" << variableName << "\" at line " << lineIndex << " while reading map file " << fileName << "." << endl;
			}
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
	for (map<string, Thingy*>::iterator iterator = models.begin(); iterator != models.end(); ++iterator)
	{
		thingyVector.push_back(iterator->second);
	}

	// return the read positioned geometries
	return Map(thingyVector, positionedThingies);
}