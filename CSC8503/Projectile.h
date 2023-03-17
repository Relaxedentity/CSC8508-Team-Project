#pragma once
#include "GameObject.h"
#include "PaintClass.h"
#include <string>
#include "Sound.h"
#include "TutorialGame.h"

namespace reactphysics3d {
	class RigidBody;
	class Transform;
	class CollisionCallback;
}

namespace NCL::CSC8503 {
	class Projectile : public PaintClass {
	public:
		Projectile(TutorialGame* t, GameWorld* world, float time, std::string name = "");
		~Projectile();
		void Reset();
		void OnCollisionBegin(GameObject* otherObject) override;
		std::vector<reactphysics3d::Vector3> CalculateSphereVertices(Vector3 hitPos);
		void Update(float dt) override;
		SoundObject* collisionV;
	private:
		float time;
		float particleTime;
		float max_time;
		reactphysics3d::Vector3 burstPos;
		vector<GameObject*> tempParticles;
		TutorialGame* tgame;
	};
}
