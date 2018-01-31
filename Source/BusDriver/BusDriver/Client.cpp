#include "Client.h"

Client::Client(void(*Print)(string), void(*CreateVehicle)(int, int, int, int, int), void(*DeleteVehicle)(int), void(*SelectOwnVehicle)(int))
{
	this->Print = Print;
	this->CreateVehicle = CreateVehicle;
	this->DeleteVehicle = DeleteVehicle;
	this->SelectOwnVehicle = SelectOwnVehicle;

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
	this->isConnected = false;
	this->Print("Connecting to " + ipAddress + " at port " + to_string(port) + "...");
	this->peer->Connect(ipAddress.c_str(), port, 0, 0);
	while (!this->isConnected)
	{
		this->Update();
	}
}

void Client::AskForVehicle(int vehicleId)
{
	this->isOwnVehicleSelected = false;
	BitStream bitStreamOut;
	bitStreamOut.Write((MessageID)ID_ASK_FOR_VEHICLE);
	bitStreamOut.Write(vehicleId);
	this->peer->Send(&bitStreamOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, this->serverAddress, false);
	while (!this->isOwnVehicleSelected)
	{
		this->Update();
	}
}

void Client::Update()
{
	for (Packet* packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			this->Print("Our connection request has been accepted.");
			this->serverAddress = packet->systemAddress;
			this->isConnected = true;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			this->Print("The server is full.");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			this->Print("We have been disconnected.");
			this->isConnected = false;
			break;
		case ID_CONNECTION_LOST:
			this->Print("Connection lost.");
			this->isConnected = false;
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			this->Print("Could not connect to server.");
			break;
		case ID_VEHICLE_CREATE:
		{
			BitStream bitStreamIn(packet->data, packet->length, false);
			bitStreamIn.IgnoreBytes(sizeof(MessageID));
			int vehicleId;
			int startingPositionX;
			int startingPositionY;
			int startingPositionZ;
			int instanceId;
			bitStreamIn.Read(vehicleId);
			bitStreamIn.Read(startingPositionX);
			bitStreamIn.Read(startingPositionY);
			bitStreamIn.Read(startingPositionZ);
			bitStreamIn.Read(instanceId);
			this->CreateVehicle(vehicleId, startingPositionX, startingPositionY, startingPositionZ, instanceId);
		}
		break;
		case ID_VEHICLE_DELETE:
		{
			BitStream bitStreamIn(packet->data, packet->length, false);
			bitStreamIn.IgnoreBytes(sizeof(MessageID));
			int instanceId;
			bitStreamIn.Read(instanceId);
			this->DeleteVehicle(instanceId);
		}
		break;
		case ID_SELECT_OWN_VEHICLE:
		{
			BitStream bitStreamIn(packet->data, packet->length, false);
			bitStreamIn.IgnoreBytes(sizeof(MessageID));
			int instanceId;
			bitStreamIn.Read(instanceId);
			this->SelectOwnVehicle(instanceId);
			this->isOwnVehicleSelected = true;
		}
		break;
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}
}