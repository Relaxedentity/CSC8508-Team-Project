#pragma once
#include "GameObject.h"
#include <string>

namespace NCL::CSC8503 {
	class PaintClass : public GameObject {
	public:
		PaintClass(GameWorld* world, std::string name = "");
		~PaintClass();
		void setPaintColour(char input);
		char getPaintColour() { return paintColour; }
	protected:
		char paintColour;
	};
}
