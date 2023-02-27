#include "PaintClass.h"
#include "CollisionDetection.h"
#include "RenderObject.h"
#include "Debug.h"

using namespace reactphysics3d;
using namespace NCL::CSC8503;

PaintClass::PaintClass(GameWorld* world, std::string name) :GameObject(world, name) {

}

PaintClass::~PaintClass() {

}

void PaintClass::setPaintColour(char input) {
	paintColour = input;
	Vector4 colourVector;
	switch (input) {
	case 'r':
		colourVector = Vector4(1, 0, 0, 1);
		break;
	case 'b':
		colourVector = Vector4(0, 0, 1, 1);
		break;
	}
	GetRenderObject()->SetColour(colourVector);
}