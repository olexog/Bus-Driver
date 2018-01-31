#include "Server.h"

Server::Server(int maxClients, int port, void(*Print)(string))
{
	this->maxClients = maxClients;
	this->Print = Print;

	this->clientAddresses = vector<SystemAddress>();

	this->peer = RakPeerInterface::GetInstance();
	SocketDescriptor socketDescriptor = SocketDescriptor(port, 0);
	this->peer->Startup(maxClients, &socketDescriptor, 1);
	this->peer->SetMaximumIncomingConnections(maxClients);
	this->Print("Starting the server.");
}

Server::~Server()
{
	RakPeerInterface::DestroyInstance(this->peer);
}

void Server::RemoveClientAddress(SystemAddress clientAddress)
{
	for (int i = 0; i < this->clientAddresses.size(); i++)
	{
		if (this->clientAddresses[i] == clientAddress)
		{
			this->clientAddresses.erase(this->clientAddresses.begin() + i);
			return;
		}
	}
}

void Server::Update()
{
	for (Packet* packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			this->Print("Another client has disconnected.");
			this->RemoveClientAddress(packet->systemAddress);
			break;
		case ID_REMOTE_CONNECTION_LOST:
			this->Print("Another client has lost the connection.");
			this->RemoveClientAddress(packet->systemAddress);
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			this->Print("Another client has connected");
			this->clientAddresses.push_back(packet->systemAddress);
			break;
		case ID_NEW_INCOMING_CONNECTION:
			this->Print("A connection is incoming.");
			this->clientAddresses.push_back(packet->systemAddress);
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			this->Print("A client has disconnected");
			this->RemoveClientAddress(packet->systemAddress);
			break;
		case ID_CONNECTION_LOST:
			this->Print("A client lost the connection.");
			this->RemoveClientAddress(packet->systemAddress);
			break;
		case ID_ASK_FOR_VEHICLE:
			break;
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}
}