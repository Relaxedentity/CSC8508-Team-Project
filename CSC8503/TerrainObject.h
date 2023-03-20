#pragma once
#include "GameObject.h"
#include "GameWorld.h"
#include <string>

namespace NCL::CSC8503 {
	class TerrainObject : public GameObject {
	public:
		TerrainObject(GameWorld* world, std::string name = "");
		~TerrainObject();
		void addPaintNode(NCL::Vector3 location);

		void paintHit(NCL::Maths::Vector3 position, char paintColour) override;

		void drawPaintNodes();

		int nodeTally() { return paintNodes.size(); }
		int redTally = 0;
		int blueTally = 0;
	protected:
		std::vector<PaintNode*> paintNodes;
	};
}
