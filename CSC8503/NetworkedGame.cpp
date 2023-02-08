#include <reactphysics3d/reactphysics3d.h>
#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "GameClient.h"
#include "PhysicsSystem.h"
#include <iostream>
#include <fstream>
#include <string>

#define COLLISION_MSG 30

struct MessagePacket : public GamePacket {
	short playerID;
	short messageID;

	MessagePacket() {
		type = Message;
		size = sizeof(short) * 2;
	}
};

NetworkedGame::NetworkedGame()	{
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket  = 0.0f;
	packetsToSnapshot = 0;
	lastID = 0;
}

NetworkedGame::~NetworkedGame()	{
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::StartAsServer() {
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);
	player2 = AddPlayer2ToWorld(reactphysics3d::Vector3(10, 5, -330), reactphysics3d::Quaternion::identity());
	thisServer->RegisterPacketHandler(Received_State, this);
	goose->setTarget2(player2);
	
	StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	thisClient = new GameClient();
	
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());
	player2 = AddPlayer2ToWorld(reactphysics3d::Vector3(10, 5, -330), reactphysics3d::Quaternion::identity());
	LockCameraToObject(player2);
	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	goose->setTarget2(player2);

	
	StartLevel();
}

void NetworkedGame::UpdateGame(float dt) {
	timeToNextPacket -= dt;
	if (timeToNextPacket < 0) {
		if (thisServer) {
			UpdateAsServer(dt);
		}
		else if (thisClient) {
			UpdateAsClient(dt);
		}
		timeToNextPacket += 1.0f / 40.0f; //20hz server/client update
	}

	if (!thisServer && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		StartAsServer();
	}
	if (!thisClient && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		StartAsClient(127,0,0,1);
	}

	TutorialGame::UpdateGame(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	MovePlayer(player, dt);
	Debug::Print(std::to_string(player2->getScore()), Vector2(15, 95), Debug::RED);
	thisServer->UpdateServer();
	

	packetsToSnapshot--;
	if (packetsToSnapshot < 0) {
		BroadcastSnapshot(false);
		packetsToSnapshot = 5;
	}
	else {
		BroadcastSnapshot(true);
	}
}

void NetworkedGame::UpdateAsClient(float dt) {
	MovePlayer(player2, dt);
	Debug::Print(std::to_string(player2->getScore()), Vector2(15, 95), Debug::RED);
	thisClient->UpdateClient();

	ClientPacket newPacket;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			//fire button pressed!
			newPacket.buttonstates[0] = 1;
			newPacket.lastID = lastID; //You'll need to work this out somehow...
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			//fire button pressed!
			newPacket.buttonstates[0] = 2;
			newPacket.lastID = lastID;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			//fire button pressed!
			newPacket.buttonstates[0] = 3;
			newPacket.lastID = lastID;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			//fire button pressed!
			newPacket.buttonstates[0] = 4;
			newPacket.lastID = lastID;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
			//fire button pressed!
			newPacket.buttonstates[0] = 5;
			newPacket.lastID = lastID;
		}
	
	lastID++;
	thisClient->SendPacket(newPacket);
}

void NetworkedGame::BroadcastSnapshot(bool deltaFrame) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		//TODO - you'll need some way of determining
		//when a player has sent the server an acknowledgement
		//and store the lastID somewhere. A map between player
		//and an int could work, or it could be part of a 
		//NetworkPlayer struct. 
		int playerState = 0;
		GamePacket* newPacket = nullptr;
		if (o->WritePacket(&newPacket, deltaFrame, playerState)) {
			thisServer->SendGlobalPacket(*newPacket);
			delete newPacket;
		}
	}
}

void NetworkedGame::UpdateMinimumState() {
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto i : stateIDs) {
		minID = std::min(minID, i.second);
		maxID = std::max(maxID, i.second);
	}
	//every client has acknowledged reaching at least state minID
	//so we can get rid of any old states!
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		o->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
	}
}

void NetworkedGame::SpawnPlayer() {

}

void NetworkedGame::StartLevel() {
	
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {
	
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		if (((FullPacket*)payload)->objectID == o->GetNetworkID()) {
			//std::cout << "server " << source << std::endl;
			o->ReadPacket(*payload);
		}
		if (type = Full_State) {
			//std::cout << ((FullPacket*)payload)->objectID << std::endl;
			//std::cout << ((FullPacket*)payload)->fullState.position << std::endl;
		}
		if (type = Received_State) {
			if (o->getGameObject().GetWorldID() == 2) {
				//std::cout << "client" << source << std::endl;
				if (((ClientPacket*)payload)->buttonstates[0] == 1) {
					o->GameobjectMove(1);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 2) {
					o->GameobjectMove(2);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 3) {
					o->GameobjectMove(3);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 4) {
					o->GameobjectMove(4);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 5) {
					o->GameobjectMove(5);
				}
			}
		}

	}
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b) {
	if (thisServer) { //detected a collision between players!
		MessagePacket newPacket;
		newPacket.messageID = COLLISION_MSG;
		newPacket.playerID  = a->GetPlayerNum();

		thisClient->SendPacket(newPacket);

		newPacket.playerID = b->GetPlayerNum();
		thisClient->SendPacket(newPacket);
	}
}