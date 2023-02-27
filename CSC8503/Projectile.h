#pragma once
#include "GameObject.h"
#include "PaintClass.h"
#include <string>

namespace reactphysics3d {
	class RigidBody;
	class Transform;
	class CollisionCallback;
}

namespace NCL::CSC8503 {
	class Projectile : public PaintClass {
	public:
		Projectile(GameWorld* world, std::string name = "");
		~Projectile();
		void OnCollisionBegin(GameObject* otherObject) override;
		void Update(float dt) override;
		float time;
	};
}
