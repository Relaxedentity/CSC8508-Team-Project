#include <reactphysics3d/body/RigidBody.h>
#include "GameObject.h"
#include "CollisionDetection.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Debug.h"
#include "GameWorld.h"
#include "PlayerObject.h"
#include "Vector3.h"
#include "Sound.h"
#include "Gamelock.h"


using namespace reactphysics3d;
using namespace NCL::CSC8503;

PlayerObject::PlayerObject(GameWorld* world, std::string name) :PaintClass(world, name) {

}

PlayerObject::~PlayerObject() {

}

void PlayerObject::OnCollisionBegin(GameObject* otherObject) {
   if (otherObject && otherObject->GetName() == "coin") {
	   voice->coin(GameLock::hitsound);
	   //std::cout << "Hit the coin£¡£¡" << std::endl;

	   //.....Player Attribute Enhancement//

	   //

	   otherObject->setActive(false);
	   otherObject->GetPhysicsObject()->setType(BodyType::STATIC);

   }
   if (otherObject && otherObject->GetName() == "capsule") {
	   voice->capsule(GameLock::hitsound);
	   //std::cout << "Hit the capsule£¡£¡" << std::endl;
	   if (this == world->GetPlayer()&&world->GetPlayerHealth()<1) {
		   world->SetPlayerHealth(world->GetPlayerHealth() + 0.2f);
		   if (world->GetPlayerHealth() > 1) world->SetPlayerHealth(1.0f);
	   }
	   else if (this == world->GetPlayerCoop() && world->GetPlayerCoopHealth() < 1) {
		   world->SetPlayerCoopHealth(world->GetPlayerCoopHealth() + 0.2f);
		   if (world->GetPlayerCoopHealth() > 1) world->SetPlayerCoopHealth(1.0f);
	   }
	   otherObject->setActive(false);
	   otherObject->GetPhysicsObject()->setType(BodyType::STATIC);
   }
   if (otherObject && otherObject->GetName() == "cat") {
	   voice->cat(GameLock::hitsound);
	   //std::cout << "Hit the cat£¡£¡" << std::endl;
	   if (this == world->GetPlayer()) {
		   setFireMode(true);
		   GameLock::p1ModeTime = 10.0f;
	   }
	   if (this == world->GetPlayerCoop()) {
		   setFireMode(true);
		   GameLock::p2ModeTime = 10.0f;
	   }
	   otherObject->setActive(false);
	   otherObject->GetPhysicsObject()->setType(BodyType::STATIC);
   }
}