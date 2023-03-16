#include "GameWorld.h"
#include "MapNode.h"
#include "Vector3.h"

using namespace NCL::CSC8503;

MapNode::MapNode(Vector3 pos) {
	location = pos;
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