#include "Server.h"

Server::Server(int maxClients, int port, void(*Print)(string))
{
	this->maxClients = maxClients;
	this->Print = Print;

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

void Server::Update()
{
	for (Packet* packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			this->Print("Another client has disconnected.");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			this->Print("Another client has lost the connection.");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			this->Print("Another client has connected");
			break;
		case ID_NEW_INCOMING_CONNECTION:
			this->Print("A connection is incoming.");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			this->Print("A client has disconnected");
			break;
		case ID_CONNECTION_LOST:
			this->Print("A client lost the connection.");
			break;
		}
	}
}