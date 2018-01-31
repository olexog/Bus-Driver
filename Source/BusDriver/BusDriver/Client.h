#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <RakNetTypes.h>

#include <string>

#include "GameMessages.h"

using namespace std;
using namespace RakNet;

class Client
{
public:
	Client(void(*Print)(string), void(*CreateVehicle)(int, int, int, int, int), void(*DeleteVehicle)(int), void(*SelectOwnVehicle)(int));
	~Client();

	void Connect(int port, string ipAddress);
	void AskForVehicle(int vehicleId);
	void Update();
private:
	RakPeerInterface* peer;
	void(*Print)(string);
	void(*CreateVehicle)(int, int, int, int, int);
	void(*DeleteVehicle)(int);
	void(*SelectOwnVehicle)(int);

	bool isConnected;
	bool isOwnVehicleSelected;
	SystemAddress serverAddress;
};

