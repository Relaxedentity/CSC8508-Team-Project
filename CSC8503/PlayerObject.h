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
	class PlayerObject : public PaintClass {
	public:
		PlayerObject(GameWorld* world, std::string name = "");
		~PlayerObject();
	};
}