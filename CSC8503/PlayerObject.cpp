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

PlayerObject::PlayerObject(GameWorld* world, std::string name) :PaintClass(world, name) {

}

PlayerObject::~PlayerObject() {

}