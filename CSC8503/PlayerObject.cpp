#include <reactphysics3d/body/RigidBody.h>
#include "GameObject.h"
#include "CollisionDetection.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Debug.h"
#include "GameWorld.h"
#include "PlayerObject.h"
#include "Vector3.h"

using namespace reactphysics3d;
using namespace NCL::CSC8503;

PlayerObject::PlayerObject(GameWorld* world, std::string name) :GameObject(world, name) {

}

PlayerObject::~PlayerObject() {

}

void PlayerObject::setPaintColour(char input) {
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