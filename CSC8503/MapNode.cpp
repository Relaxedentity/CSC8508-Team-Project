#include "GameWorld.h"
#include "MapNode.h"
#include "Vector3.h"
#

using namespace NCL::CSC8503;

MapNode::MapNode(Vector3 pos, bool floorIn) {
	location = pos;
	isFloor = floorIn;
}

MapNode::~MapNode() {

}

void MapNode::addObject(TerrainObject* i) {
	objects.push_back(i);
}

void MapNode::drawNodes() {
	for (auto& i : objects) {
		i->drawPaintNodes();
	}
}

float MapNode::getColourRatio() {
	int nodeTally = 0;
	int redCount = 0;
	int blueCount = 0;
	for (auto& i : objects) {
		nodeTally += i->nodeTally();
		redCount += i->redTally;
		blueCount += i->blueTally;
	}
	return (redCount - blueCount) / nodeTally;
}