#include <reactphysics3d/reactphysics3d.h>
#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "GameClient.h"
#include "RenderObject.h"
#include "Debug.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Projectile.h"
#include "Gamelock.h"

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
	animatedShaderB = new OGLShader("SkinningVertex.glsl", "SkinningFrag.glsl");
	animatedShaderC = new OGLShader("SkinningVertex.glsl", "SkinningFrag.glsl");
	animatedShaderD = new OGLShader("SkinningVertex.glsl", "SkinningFrag.glsl");
}

NetworkedGame::~NetworkedGame() {
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::StartAsServer() {
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);
	player2 = AddPlayerToWorld(reactphysics3d::Vector3(55, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderB, 'b', 2, 2);
	player3 = AddPlayerToWorld(reactphysics3d::Vector3(60, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderC, 'r', 3, 3);
	player4 = AddPlayerToWorld(reactphysics3d::Vector3(65, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderD, 'b', 4, 4);
	player2->SetPlayerHealth(1.0f);
	player3->SetPlayerHealth(1.0f);
	player4->SetPlayerHealth(1.0f);
	thisServer->RegisterPacketHandler(Received_State, this);
	isMultiplayer = true;
	//goose->setTarget2(player2);
	
	//StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	thisClient = new GameClient();
	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());
	player2 = AddPlayerToWorld(reactphysics3d::Vector3(55, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderB, 'b', 2, 2);
	player3 = AddPlayerToWorld(reactphysics3d::Vector3(60, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderC, 'r', 3, 3);
	player4 = AddPlayerToWorld(reactphysics3d::Vector3(65, 2, 20), reactphysics3d::Quaternion::identity(), animatedShaderD, 'b', 4, 4);
	isMultiplayer = true;
	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(Projectile_Fired, this);
	thisClient->RegisterPacketHandler(Health, this);
	thisClient->RegisterPacketHandler(Movement, this);
	//goose->setTarget2(thisPlayer);


	//StartLevel();
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
		timeToNextPacket += 1.0f / 20.0f; //20hz server/client update
	}

	if (!thisServer && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9) && !initSplitScreen) {
		GameLock::isNetwork = true;
		StartAsServer();
	}
	if (!thisClient && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10) && !initSplitScreen) {
		GameLock::isNetwork = true;
		StartAsClient(127, 0, 0, 1);
	}
	TutorialGame::UpdateGame(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	UpdateNetworkAnimations(dt);
	updateCamera(player, dt);
	CheckGrounded(player);
	world->playerHealth = player->GetPlayerHealth();
	moveDesignatedPlayer(player, dt, world->GetMainCamera()->GetPosition());
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F)) {
		BroadcastProjectile(player);
	}
	//MovePlayer(player, dt);
	thisServer->UpdateServer();
	ResetMovementFrame(player2);
	ResetMovementFrame(player3);
	ResetMovementFrame(player4);
	FreezingPlayers(dt, player2);
	FreezingPlayers(dt, player3);
	FreezingPlayers(dt, player4);
	PlayerPaintTracks(player, 'r');
	PlayerPaintTracks(player2, 'b');
	PlayerPaintTracks(player3, 'r');
	PlayerPaintTracks(player4, 'b');
	packetsToSnapshot--;
	if (packetsToSnapshot < 0) {
		BroadcastPlayerHealth(player2);
		BroadcastPlayerHealth(player3);
		BroadcastPlayerHealth(player4);
		//std::cout <<"phealth : " << player2->GetPlayerHealth() << std::endl;
		BroadcastSnapshot(false);
		packetsToSnapshot = 5;
	}
	else {
		BroadcastSnapshot(true);
	}
}

void NetworkedGame::UpdateAsClient(float dt) {
	reactphysics3d::Quaternion yaw;
	reactphysics3d::Quaternion pitch;
	bool grounded = false;
	UpdateNetworkAnimations(dt);
	CheckGrounded(player2);
	CheckGrounded(player3);
	CheckGrounded(player4);
	switch (thisClient->clientID) {
	case 1:
		updateCamera(player2, dt);
		world->playerHealth = player2->GetPlayerHealth();
		CheckGrounded(player2);
		yaw = player2->GetYaw();
		pitch = player2->GetPitch();
		grounded = player2->IsGrounded();
		ResetMovementFrame(player3);
		ResetMovementFrame(player4);
		break;
	case 2:
		updateCamera(player3, dt);
		world->playerHealth = player3->GetPlayerHealth();
		CheckGrounded(player3);
		yaw = player3->GetYaw();
		pitch = player3->GetPitch();
		grounded = player3->IsGrounded();
		ResetMovementFrame(player2);
		ResetMovementFrame(player4);
		break;
	case 3:
		updateCamera(player4, dt);
		world->playerHealth = player4->GetPlayerHealth();
		CheckGrounded(player4);
		yaw = player4->GetYaw();
		pitch = player4->GetPitch();
		grounded = player4->IsGrounded();
		ResetMovementFrame(player2);
		ResetMovementFrame(player3);
		break;
	}
	ResetMovementFrame(player);
	player2->directionInput = false;
	player3->directionInput = false;
	player4->directionInput = false;
	thisClient->UpdateClient();
	PlayerPaintTracks(player, 'r');
	PlayerPaintTracks(player2, 'b');
	PlayerPaintTracks(player3, 'r');
	PlayerPaintTracks(player4, 'b');
	ClientPacket newPacket;
	newPacket.myID = thisClient->clientID;
	if (world->playerHealth > 0) {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
			newPacket.buttonstates[0] = 1;
			newPacket.lastID = lastID;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
			newPacket.buttonstates[0] = 2;
			newPacket.lastID = lastID;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
			newPacket.buttonstates[0] = 3;
			newPacket.lastID = lastID;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
			newPacket.buttonstates[0] = 4;
			newPacket.lastID = lastID;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			newPacket.buttonstates[0] = 5;
			newPacket.lastID = lastID;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F)) {
			newPacket.buttonstates[0] = 6;
			newPacket.lastID = lastID;
		}
	}
	newPacket.yaw[0] = yaw.x;
	newPacket.yaw[1] = yaw.y;
	newPacket.yaw[2] = yaw.z;
	newPacket.yaw[3] = yaw.w;
	newPacket.yaw[4] = grounded ? 1 : 0;
	newPacket.yaw[5] = pitch.x;
	newPacket.yaw[6] = pitch.y;
	newPacket.yaw[7] = pitch.z;
	newPacket.yaw[8] = pitch.w;
	//std::cout << goatRot << "\n";

	
	lastID++;
	thisClient->SendPacket(newPacket);
}

void NetworkedGame::BroadcastClientMovement(int ID,int buttonState, Quaternion yaw, bool grounded) {
	MovementPacket newPacket;
	newPacket.myID = ID;
	newPacket.buttonstates[0] = buttonState;
	newPacket.yaw[0] = yaw.x;
	newPacket.yaw[1] = yaw.y;
	newPacket.yaw[2] = yaw.z;
	newPacket.yaw[3] = yaw.w;
	newPacket.yaw[4] = grounded ? 1 : 0;
	newPacket.lastID = lastID;
	lastID++;
	thisServer->SendGlobalPacket(newPacket);
}

void NetworkedGame::BroadcastProjectile(PlayerObject* p) {
	ProjectilePacket newPacket;
	reactphysics3d::Quaternion pitch;
	pitch = p->GetPitch();
	newPacket.lastID = lastID;
	newPacket.myID = p->GetWorldID();
	newPacket.yaw[5] = pitch.x;
	newPacket.yaw[6] = pitch.y;
	newPacket.yaw[7] = pitch.z;
	newPacket.yaw[8] = pitch.w;
	lastID++;

	thisServer->SendGlobalPacket(newPacket);
}

void NetworkedGame::BroadcastPlayerHealth(PlayerObject* p) {
	HealthPacket newPacket;
	newPacket.lastID = lastID;
	newPacket.myID = p->GetWorldID();
	newPacket.health = p->GetPlayerHealth();
	lastID++;

	thisServer->SendGlobalPacket(newPacket);
}

void NetworkedGame::BroadcastSnapshot(bool deltaFrame) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	player->playerState = lastID;
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
		GamePacket* newPacket = nullptr;
		if (o->WritePacket(&newPacket, deltaFrame, player->playerState)) {
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
			if (o->GetNetworkID() == 10) {
				reactphysics3d::Vector3 temp = reactphysics3d::Vector3(player2->GetPhysicsObject()->getTransform().getPosition()) + reactphysics3d::Vector3(1, 2, 0);
				//Debug::DrawLine(Vector3(temp), Vector3(temp) - Vector3(0, 2, 0), Debug::BLUE);
				//projectiles[9]->GetPhysicsObject()->setTransform(reactphysics3d::Transform(temp, reactphysics3d::Quaternion::identity()));
				//std::cout << o->getGameObject().GetPhysicsObject()->getTransform().getPosition().x<<","<< o->getGameObject().GetPhysicsObject()->getTransform().getPosition().y<<","<< o->getGameObject().GetPhysicsObject()->getTransform().getPosition().z << std::endl;
			}
			if (((FullPacket*)payload)->fullState.stateID > lastID) {
				lastID = ((FullPacket*)payload)->fullState.stateID;
			}
			
		}
		if (((DeltaPacket*)payload)->objectID == o->GetNetworkID()) {
			o->ReadPacket(*payload);
		}
		if (type == Health) {
			if (o->getGameObject().GetWorldID() == ((HealthPacket*)payload)->myID) {
				((PlayerObject*)(&(o->getGameObject())))->SetPlayerHealth(((HealthPacket*)payload)->health);
				//std::cout<< "ID: " << ((HealthPacket*)payload)->myID << "  hpacket : " << ((HealthPacket*)payload)->health << std::endl;

			}
		}
		if (type == Projectile_Fired) {
			if (o->getGameObject().GetWorldID() == ((ProjectilePacket*)payload)->myID) {
				//std::cout << ((ProjectilePacket*)payload)->myID;
				Quaternion pitch = Quaternion(((ProjectilePacket*)payload)->yaw[5], ((ProjectilePacket*)payload)->yaw[6], ((ProjectilePacket*)payload)->yaw[7], ((ProjectilePacket*)payload)->yaw[8]);
				ShootProjectile((PlayerObject*)(&(o->getGameObject())), pitch);
			}
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
		if (type == Movement) {
			Quaternion yaw = Quaternion(((ClientPacket*)payload)->yaw[0], ((ClientPacket*)payload)->yaw[1], ((ClientPacket*)payload)->yaw[2], ((ClientPacket*)payload)->yaw[3]);
			bool grounded = ((ClientPacket*)payload)->yaw[4] == 1 ? true : false;
			if (o->getGameObject().GetWorldID() == ((MovementPacket*)payload)->myID) {
				if (((MovementPacket*)payload)->buttonstates[0] == 1) {
					MoveForward((PlayerObject*)(&(o->getGameObject())), yaw, Vector3(0, 0, 0), grounded);
					((PlayerObject*)(&(o->getGameObject())))->directionInput = true;
				}
				if (((MovementPacket*)payload)->buttonstates[0] == 2) {
					MoveBackward((PlayerObject*)(&(o->getGameObject())), yaw, Vector3(0, 0, 0), grounded);
					((PlayerObject*)(&(o->getGameObject())))->directionInput = true;
				}
				if (((MovementPacket*)payload)->buttonstates[0] == 3) {
					MoveRight((PlayerObject*)(&(o->getGameObject())), yaw, Vector3(0, 0, 0), grounded);
					((PlayerObject*)(&(o->getGameObject())))->directionInput = true;
				}
				if (((MovementPacket*)payload)->buttonstates[0] == 4) {
					MoveLeft((PlayerObject*)(&(o->getGameObject())), yaw, Vector3(0, 0, 0), grounded);
					((PlayerObject*)(&(o->getGameObject())))->directionInput = true;
				}
				if (((MovementPacket*)payload)->buttonstates[0] == 5) {
					MoveJump((PlayerObject*)(&(o->getGameObject())));
					//((PlayerObject*)(&(o->getGameObject())))->directionInput = true;
				}
			}
		}
		if (type == Received_State) {
			if (o->getGameObject().GetWorldID() == ((ClientPacket*)payload)->myID + 1) {
				player->playerState = ((ClientPacket*)payload)->lastID;
				o->getGameObject().directionInput = false;
				reactphysics3d::Vector3 temp = reactphysics3d::Vector3(player2->GetPhysicsObject()->getTransform().getPosition()) + reactphysics3d::Vector3(1, 2, 0);
				Quaternion yaw = Quaternion(((ClientPacket*)payload)->yaw[0], ((ClientPacket*)payload)->yaw[1], ((ClientPacket*)payload)->yaw[2], ((ClientPacket*)payload)->yaw[3]);
				Quaternion pitch = Quaternion(((ClientPacket*)payload)->yaw[5], ((ClientPacket*)payload)->yaw[6], ((ClientPacket*)payload)->yaw[7], ((ClientPacket*)payload)->yaw[8]);
				bool grounded = ((ClientPacket*)payload)->yaw[4] == 1 ? true : false;
				o->getGameObject().GetPhysicsObject()->resetTorque();
				o->getGameObject().SetPitch(reactphysics3d::Quaternion(pitch.x,pitch.y,pitch.z,pitch.w));
				o->getGameObject().setGrounded(grounded);
				o->getGameObject().SetYaw(reactphysics3d::Quaternion(yaw.x, yaw.y, yaw.z, yaw.w));
				if (((ClientPacket*)payload)->buttonstates[0] == 1) {
					o->GameobjectMove(1, yaw, grounded);
					BroadcastClientMovement(((ClientPacket*)payload)->myID+1,1, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 2) {
					o->GameobjectMove(2, yaw, grounded);
					BroadcastClientMovement(((ClientPacket*)payload)->myID + 1, 2, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 3) {
					o->GameobjectMove(3, yaw, grounded);
					BroadcastClientMovement(((ClientPacket*)payload)->myID + 1, 3, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 4) {
					o->GameobjectMove(4, yaw, grounded);
					BroadcastClientMovement(((ClientPacket*)payload)->myID + 1, 4, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 5) {
					o->GameobjectMove(5, yaw, grounded);
					BroadcastClientMovement(((ClientPacket*)payload)->myID + 1, 5, yaw, grounded);
				}
				if (((ClientPacket*)payload)->buttonstates[0] == 6) {
					ShootProjectile((PlayerObject*)(&(o->getGameObject())), pitch);
					BroadcastProjectile((PlayerObject*)(&(o->getGameObject())));
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

void  NetworkedGame::UpdateNetworkAnimations(float dt) {
	player2->GetRenderObject()->frameTime -= dt;
	player3->GetRenderObject()->frameTime -= dt;
	player4->GetRenderObject()->frameTime -= dt;

	if (player2->directionInput) {
		UpdateAnim(player2, playerWalkAnim);
	}
	else {
		UpdateAnim(player2, playerIdleAnim);
	}
	if (player3->directionInput) {
		UpdateAnim(player3, playerWalkAnim);
	}
	else {
		UpdateAnim(player3, playerIdleAnim);
	}
	if (player4->directionInput) {
		UpdateAnim(player4, playerWalkAnim);
	}
	else {
		UpdateAnim(player4, playerIdleAnim);
	}
}
void  NetworkedGame::ResetMovementFrame(GameObject* p) {
	if (!p->directionInput) {
		//p->GetPhysicsObject()->resetForce();
		//p->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(0, 0, 0));
	}
}
void NetworkedGame::NetworkPaintTracks() {
	PlayerPaintTracks(player2, 'b');
	PlayerPaintTracks(player3, 'r');
	PlayerPaintTracks(player4, 'b');
}

