#pragma once
#include "GameObject.h"
#include <string>

namespace reactphysics3d {
	class RigidBody;
	class Transform;
	class CollisionCallback;
}

namespace NCL::CSC8503 {
	class PlayerObject : public GameObject {
	public:
		PlayerObject(GameWorld* world, std::string name = "");
		~PlayerObject();
		void setPaintColour(char input) { paintColour = input; }
		char getPaintColour() { return paintColour; }
	protected:
		char paintColour;
	};
}

