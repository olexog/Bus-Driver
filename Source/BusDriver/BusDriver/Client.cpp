#include "Client.h"

Client::Client(void(*Print)(string), void(*CreateVehicle)(int, int, int, int), void(*DeleteVehicle)(int))
{
	this->Print = Print;
	this->CreateVehicle = CreateVehicle;
	this->DeleteVehicle = DeleteVehicle;

	this->peer = RakPeerInterface::GetInstance();
	SocketDescriptor socketDescriptor;
	this->peer->Startup(1, &socketDescriptor, 1);
	this->Print("Starting the client.");
}

Client::~Client()
{
	RakPeerInterface::DestroyInstance(this->peer);
}

void Client::Connect(int port, string ipAddress)
{
	this->Print("Connecting to " + ipAddress + " at port " + to_string(port) + "...");
	this->peer->Connect(ipAddress.c_str(), port, 0, 0);
}

void Client::Update()
{
	for (Packet* packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			this->Print("Our connection request has been accepted.");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			this->Print("The server is full.");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			this->Print("We have been disconnected.");
			break;
		case ID_CONNECTION_LOST:
			this->Print("Connection lost.");
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			this->Print("Could not connect to server.");
			break;
		}
	}
}
