#pragma once
#include "GameObject.h"
#include "PaintClass.h"
#include "Sound.h"
#include <string>

namespace reactphysics3d {
	class RigidBody;
	class Transform;
	class CollisionCallback;
}

namespace NCL::CSC8503 {
	class PlayerObject : public PaintClass {
	public:
		SoundObject* voice;
		int playerState;
		PlayerObject(GameWorld* world, std::string name = "");
		~PlayerObject();
		Vector3 currentPos = Vector3(0, 0, 0);
		void OnCollisionBegin(GameObject* otherObject) override;
		
		/// ShotGun Mode ///
		void setFireMode(bool setShotGun) { isShotGun = setShotGun; };
		bool getFireMode() { return isShotGun; };

		/// Accelerated Mode ///
		void setACARMode(bool setACAR) { isACARMode = setACAR; };
		bool getACARMode() { return isACARMode; };
		void SetPlayerHealth(float health) {
			playerHealth = health;
		}
		float GetPlayerHealth() {
			return playerHealth;
		}
		bool playerLocked = false;
		float freezeTimer = 3.0f;
	protected:
		bool isShotGun = false;
		bool isACARMode = false;
		float playerHealth;
		
	};

}