#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <string>
#include <vector>

#include "GameMessages.h"

using namespace std;
using namespace RakNet;

class Server
{
public:
	Server(int maxClients, int port, void (*Print)(string));
	~Server();

	void Update();
private:
	void RemoveClientAddress(SystemAddress clientAddress);

	RakPeerInterface* peer;
	int maxClients;
	void (*Print)(string);

	vector<SystemAddress> clientAddresses;

};

