#pragma once
#include "GameWorld.h"
#include "TerrainObject.h"

namespace NCL::CSC8503 {
	class MapNode {
	public:
		MapNode(Vector3 pos, bool floorIn);
		~MapNode();
		Vector3 location;
		//Vector4 getColour();
		void addObject(TerrainObject* i);
		void drawNodes();
		float getColourRatio();
		bool isFloor;
	protected:
		std::vector<TerrainObject*> objects;
	};
}