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

	   if (this == world->GetPlayer()) {
		   setACARMode(true);
		   GameLock::p1FastTime = 10.0f;
	   }
	   if (this == world->GetPlayerCoop()) {
		   setACARMode(true);
		   GameLock::p2FastTime = 10.0f;
	   }

	   otherObject->setActive(false);
	   otherObject->GetPhysicsObject()->setType(BodyType::STATIC);

   }
   if (otherObject && otherObject->GetName() == "capsule") {
	   voice->capsule(GameLock::hitsound);
	   //std::cout << "Hit the capsule£¡£¡" << std::endl;
	   PlayerObject* p = (PlayerObject*)(world->GetPlayer());
	   PlayerObject* cp = (PlayerObject*)(world->GetPlayerCoop());
	   if (this == p && p->GetPlayerHealth() <1) {
		   p->SetPlayerHealth(p->GetPlayerHealth() + 0.2f);
		   if (p->GetPlayerHealth() > 1) p->SetPlayerHealth(1.0f);
	   }
	   else if (this == cp && cp->GetPlayerHealth() < 1) {
		   cp->SetPlayerHealth(cp->GetPlayerHealth() + 0.2f);
		   if (cp->GetPlayerHealth() > 1) cp->SetPlayerHealth(1.0f);
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

void NCL::CSC8503::PlayerObject::OnCollisionEnd(GameObject* otherObject)
{
	if (otherObject && otherObject->GetTag() == 666) {
		if (this == world->GetPlayer() && world->GetPlayerHealth() > 0) {
				// Calculate collision magnitude
				Vector3 collisionVelocity = otherObject->GetPhysicsObject()->getLinearVelocity() - this->GetPhysicsObject()->getLinearVelocity();
				float collisionMagnitude = collisionVelocity.Length();

				// Calculate damage based on collision magnitude
				float damage = collisionMagnitude * 0.003;
				world->SetPlayerHealth(world->GetPlayerHealth() - damage);
		}
	}

}
