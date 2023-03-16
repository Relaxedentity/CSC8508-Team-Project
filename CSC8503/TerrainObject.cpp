#include "TerrainObject.h"
#include "CollisionDetection.h"
#include "RenderObject.h"
#include "Debug.h"

using namespace reactphysics3d;
using namespace NCL::CSC8503;

TerrainObject::TerrainObject(GameWorld* world, std::string name) :GameObject(world, name) {

}

TerrainObject::~TerrainObject() {

}

void TerrainObject::addPaintNode(NCL::Vector3 location) {
	PaintNode* node = new PaintNode(location);
	paintNodes.emplace_back(node);
}

void TerrainObject::paintHit(NCL::Maths::Vector3 position, char paintColour) {
	int redCounter = 0;
	int blueCounter = 0;
	for (auto& i : paintNodes) {
		char iColour = i->getColour();
		if ((position - i->getPos()).Length() < 2.5 && iColour != paintColour) {
			//std::cout << "we got a hit!\n";
			i->setColour(paintColour);
			//std::cout << "node colour: " << i->getColour() << "\n";
			if (paintColour == 'r') {
				redCounter++;
				blueCounter = (iColour != 'b') ? blueCounter : blueCounter--;
			}
			if (paintColour == 'b') {
				blueCounter++;
				redCounter = (iColour != 'r') ? redCounter : redCounter--;
			}
		}
	}
	if (redCounter != 0) {
		world->paintTally(redCounter, blueCounter);
	}
}

void TerrainObject::drawPaintNodes() {
	for (auto& i : paintNodes) {
		Vector4 colourVector;
		switch (i->getColour()) {
		case 'r':
			colourVector = Vector4(1, 0, 0, 1);
			break;
		case 'b':
			colourVector = Vector4(0, 0, 1, 1);
			break;
		}

		Debug::DrawPoint(i->getPos(), colourVector, 0.1f);
	}
}
