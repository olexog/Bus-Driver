#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <string>

using namespace std;
using namespace RakNet;

class Client
{
public:
	Client(void(*Print)(string));
	~Client();

	void Connect(int port, string ipAddress);
	void Update();
private:
	RakPeerInterface* peer;
	void(*Print)(string);
};

