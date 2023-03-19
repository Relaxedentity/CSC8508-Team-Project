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
	protected:
		bool isShotGun = false;
		bool isACARMode = false;
	};

}