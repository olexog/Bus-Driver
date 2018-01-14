#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <string>

using namespace std;
using namespace RakNet;

class Server
{
public:
	Server(int maxClients, int port, void (*Print)(string));
	~Server();

	void Update();
private:
	RakPeerInterface* peer;
	int maxClients;
	void (*Print)(string);
};

