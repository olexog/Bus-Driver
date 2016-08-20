#include "BodyReader.h"

namespace PhysicsLibrary
{
	///<summary>Represents the kind of a PxGeometry instance.</summary>
	enum class GeometryType
	{
		///<summary>Represents no geometry type.</summary>
		None,
		///<summary>Represents the PxBoxGeometry type.</summary>
		Box
	};

	///<summary>Returns a string representation of the given GeometryType enumeration instance.</summary>
	string ToString(GeometryType geometryType)
	{
		switch (geometryType)
		{
		case GeometryType::Box:
			return "box";
		default:
			return "unknown";
		}
	}

	///<summary>Represents the whitespace characters.</summary>
	const char* WHITESPACES = " \t\r\n";

	Body* BodyReader::Read(Physics* physics, string fileName)
	{
		// initialize shape vector
		vector<Shape*> result = vector<Shape*>();

		// open file stream
		ifstream file = ifstream(fileName);

		// initialize variables necessary to read
		int lineIndex = 0;
		string line;
		char command[256];

		// initialize actual geometry type to the default value
		GeometryType actualGeometry = GeometryType::None;

		// initialize geometry parameters
		vec3* position;
		PxVec3 halfExtends;

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

			// if command is "create"
			if (strcmp(command, "create") == 0)
			{
				// read geometry type
				char geometryType[256];
				sscanf_s(token, "%255s", geometryType, 256);

				// if geometry type is "box"
				if (strcmp(geometryType, "eBOX") == 0)
				{
					// set actual geometry type to box
					actualGeometry = GeometryType::Box;

					// set box parameters to the default values
					position = new vec3();
					halfExtends = PxVec3();
				}
				// if geometry type is unknown
				else
				{
					cout << "Unable to create geometry due to unknown geometry type \"" << geometryType << "\" at line " << lineIndex << " while reading body file " << fileName << "." << endl;
				}
			}
			// if command is "let"
			else if (strcmp(command, "let") == 0)
			{
				// read geometry parameter
				char geometryParameter[256];
				sscanf_s(token, "%255s", geometryParameter, 256);
				token += strlen(geometryParameter);
				// skip whitespaces
				token += strspn(token, WHITESPACES);

				// if geometry parameter is "position"
				if (strcmp(geometryParameter, "position") == 0)
				{
					// if geometry parameter is valid for the actual geometry
					if (actualGeometry == GeometryType::Box)
					{
						// read "be"
						char substantiveVerb[256];
						sscanf_s(token, "%255s", substantiveVerb, 256);
						token += strlen(substantiveVerb);
						// skip whitespaces
						token += strspn(token, WHITESPACES);

						// is substantive verb is not "be"
						if (strcmp(substantiveVerb, "be") != 0)
						{
							cout << "\"be\" expected after " << ToString(actualGeometry) << " parameter \"" << geometryParameter << "\" at line " << lineIndex << " while reading body file " << fileName << "." << endl;
						}

						// read position vector
						float* x = new float();
						float* y = new float();
						float* z = new float();
						sscanf_s(token, "(%f; %f; %f)", x, y, z);
						// set position vector
						position = new vec3(*x, *y, *z);
					}
					// if geometry parameter is invalid for the actual geometry
					else
					{
						cout << "Unknown " << ToString(actualGeometry) << " parameter \"" << geometryParameter << "\" at line " << lineIndex << " while reading body file " << fileName << "." << endl;
					}
				}
				// if geometry parameter is "halfExtends"
				else if (strcmp(geometryParameter, "halfExtends") == 0)
				{
					// if geometry parameter is valid for the actual geometry
					if (actualGeometry == GeometryType::Box)
					{
						// read "be"
						char substantiveVerb[256];
						sscanf_s(token, "%255s", substantiveVerb, 256);
						token += strlen(substantiveVerb);
						// skip whitespaces
						token += strspn(token, WHITESPACES);

						// is substantive verb is not "be"
						if (strcmp(substantiveVerb, "be") != 0)
						{
							cout << "\"be\" expected after " << ToString(actualGeometry) << " parameter \"" << geometryParameter << "\" at line " << lineIndex << " while reading body file " << fileName << "." << endl;
						}

						// read half extends
						float* x = new float();
						float* y = new float();
						float* z = new float();
						sscanf_s(token, "(%f; %f; %f)", x, y, z);
						// set half extends
						halfExtends = PxVec3(*x, *y, *z);
					}
				}
				// if geometry parameter is unknown
				else
				{
					cout << "Unknown " << ToString(actualGeometry) << " parameter \"" << geometryParameter << "\" at line " << lineIndex << " while reading body file " << fileName << "." << endl;
				}
			}
			// if command is "add"
			else if (strcmp(command, "add") == 0)
			{
				// if actual geometry type is box
				if (actualGeometry == GeometryType::Box)
				{
					// add positioned geometry
					PxGeometry* geometry = new PxBoxGeometry(halfExtends.x, halfExtends.y, halfExtends.z);
					result.push_back(new Shape(physics, geometry, position, new quat(), Shape::Type::OBSTACLE));

					// set actual geometry type to the default value
					actualGeometry = GeometryType::None;
				}
				// if actual geometry type is at the default value
				else
				{
					cout << "Unable to add nothing at line " << lineIndex << " while reading body file " << fileName << "." << endl;
				}
			}
			// if command is unknown
			else
			{
				cout << "Unknown command \"" << command << "\" at line " << lineIndex << " while reading body file " << fileName << "." << endl;
			}
		}

		// return the read positioned geometries
		return new Body(result);
	}
}