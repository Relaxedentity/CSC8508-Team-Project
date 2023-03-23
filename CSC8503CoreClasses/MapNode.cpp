#include "GameWorld.h"
#include "MapNode.h"
#include "Vector3.h"
#include "Vector4.h"

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
	return (float)(redCount - blueCount) / nodeTally;
}

Vector4 MapNode::getMapColour(bool isloading) {
	int nodeTally = 0;
	int redCount = 0;
	int blueCount = 0;
	for (auto& i : objects) {
		nodeTally += i->nodeTally();
		redCount += i->redTally;
		blueCount += i->blueTally;
	}
	float fill = 1 - ((float)(redCount + blueCount) / nodeTally);
	float ratio = (((float)(redCount - blueCount) / nodeTally)+1)/2;
	Vector4 baseline = (isFloor) ? Vector4(1.0f, 1.0f, 1.0f, 1.0f) : Vector4(0.3f, 0.3f, 0.3f, 1.0f);
	Vector4 finalOut = (baseline * fill) + (Vector4(ratio, 0.0f, 1 - ratio, 1.0f) * (1 - fill));
	if (isloading)
		finalOut = baseline;
	else
		finalOut = (isFloor) ? finalOut : finalOut * 0.75;
	return finalOut;
}