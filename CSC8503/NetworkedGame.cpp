#include <reactphysics3d/reactphysics3d.h>
#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "GameClient.h"
#include "Debug.h"
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

NetworkedGame::NetworkedGame() {
	thisServer = nullptr;
	thisClient = nullptr;
	netID = 1;
	worldCount = 1;
	playerDistance = 10;
	NetworkBase::Initialise();
	timeToNextPacket = 0.0f;
	packetsToSnapshot = 0;
	lastID = 0;
}

NetworkedGame::~NetworkedGame() {
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::StartAsServer() {
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);
	player2 = AddPlayerToWorld(reactphysics3d::Vector3(55, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderA, 2, 2);
	player3 = AddPlayerToWorld(reactphysics3d::Vector3(60, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderA, 3, 3);
	player4 = AddPlayerToWorld(reactphysics3d::Vector3(65, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderA, 4, 4);
	thisServer->RegisterPacketHandler(Received_State, this);
	//goose->setTarget2(player2);
	
	StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	thisClient = new GameClient();
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());
	player2 = AddPlayerToWorld(reactphysics3d::Vector3(55, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderA, 2, 2);
	player3 = AddPlayerToWorld(reactphysics3d::Vector3(60, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderA, 3, 3);
	player4 = AddPlayerToWorld(reactphysics3d::Vector3(65, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderA, 4, 4);

	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	//goose->setTarget2(thisPlayer);


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

	if (!thisServer && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9) && !initSplitScreen) {
		StartAsServer();
		coopMode = false;
		
	}
	if (!thisClient && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10) && !initSplitScreen) {
		StartAsClient(127, 0, 0, 1);
		coopMode = false;
	}

	TutorialGame::UpdateGame(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	MovePlayer(player, dt);
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
	reactphysics3d::Quaternion yaw;
	bool grounded = false;
	switch (thisClient->clientID) {
	case 1:
		MovePlayer(player2, dt);
		yaw = player2->GetYaw();
		grounded = player2->IsGrounded();
		break;
	case 2:
		MovePlayer(player3, dt); 
		yaw = player3->GetYaw();
		grounded = player3->IsGrounded();
		break;
	case 3:
		MovePlayer(player4, dt);
		yaw = player4->GetYaw();
		grounded = player4->IsGrounded();
		break;
	}
	thisClient->UpdateClient();

	ClientPacket newPacket;
	newPacket.myID = thisClient->clientID;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		//fire button pressed!
		newPacket.buttonstates[0] = 1;
		newPacket.lastID = lastID; //You'll need to work this out somehow...
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		//fire button pressed!
		newPacket.buttonstates[0] = 2;
		newPacket.lastID = lastID;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		//fire button pressed!
		newPacket.buttonstates[0] = 3;
		newPacket.lastID = lastID;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		//fire button pressed!
		newPacket.buttonstates[0] = 4;
		newPacket.lastID = lastID;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		//fire button pressed!
		newPacket.buttonstates[0] = 5;
		newPacket.lastID = lastID;
	}
	newPacket.yaw[0] = yaw.x;
	newPacket.yaw[1] = yaw.y;
	newPacket.yaw[2] = yaw.z;
	newPacket.yaw[3] = yaw.w;
	newPacket.yaw[4] = grounded ? 1 : 0;
	//std::cout << goatRot << "\n";

	
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
	/*player2 = AddPlayerToWorld(Vector3(10, 5, -330), 2, 2);
	player3 = AddPlayerToWorld(Vector3(15, 5, -330), 3, 3);
	player4 = AddPlayerToWorld(Vector3(20, 5, -330), 4, 4);*/
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
		if (type == Full_State) {
			//std::cout << ((FullPacket*)payload)->objectID << std::endl;
			//std::cout << ((FullPacket*)payload)->fullState.position << std::endl;
		}
		if (type == Player_Connected) {
			if (thisClient->clientID <= 0) {
				std::cout << ((InitialPacket*)payload)->count << "\n";
				thisClient->clientID = ((InitialPacket*)payload)->count;
				switch (thisClient->clientID) {
				case 1:
					LockCameraToObject(player2);
					break;
				case 2:
					LockCameraToObject(player3);
					break;
				case 3:
					LockCameraToObject(player4);
					break;
				}
			}
		}
		if (type == Received_State) {
			if (o->getGameObject().GetWorldID() == ((ClientPacket*)payload)->myID + 1) {
				//std::cout << "client" << source << std::endl;
				// o->GameObjectRotate(reactphysics3d::Quaternion(((ClientPacket*)payload)->orientation[0],
				//((ClientPacket*)payload)->orientation[1], ((ClientPacket*)payload)->orientation[2], ((ClientPacket*)payload)->orientation[3]));
				//std::cout << ((ClientPacket*)payload)->orientation[0] << ((ClientPacket*)payload)->orientation[1] << ((ClientPacket*)payload)->orientation[2] << ((ClientPacket*)payload)->orientation[3];
				Quaternion yaw = Quaternion(((ClientPacket*)payload)->yaw[0], ((ClientPacket*)payload)->yaw[1], ((ClientPacket*)payload)->yaw[2], ((ClientPacket*)payload)->yaw[3]);
				bool grounded = ((ClientPacket*)payload)->yaw[4] == 1 ? true : false;
				if (((ClientPacket*)payload)->buttonstates[0] == 1) {
					std::cout << "poop 1\n";
					o->GameobjectMove(1, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 2) {
					std::cout << "poop 2\n";
					o->GameobjectMove(2, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 3) {
					std::cout << "poop 3\n";
					o->GameobjectMove(3, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 4) {
					std::cout << "poop 4\n";
					o->GameobjectMove(4, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 5) {
					std::cout << "poop 5\n";
					o->GameobjectMove(5, yaw, grounded);
				}	
				o->GameObjectRotate(yaw);
			}
		}

	}
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b) {
	if (thisServer) { //detected a collision between players!
		MessagePacket newPacket;
		newPacket.messageID = COLLISION_MSG;
		newPacket.playerID = a->GetPlayerNum();

		thisClient->SendPacket(newPacket);

		newPacket.playerID = b->GetPlayerNum();
		thisClient->SendPacket(newPacket);
	}
}