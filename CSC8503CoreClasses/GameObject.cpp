#include <reactphysics3d/body/RigidBody.h>
#include "GameObject.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

using namespace reactphysics3d;
using namespace NCL::CSC8503;

NCL::CSC8503::GameObject::GameObject(string objectName)	{
	name			= objectName;
	worldID			= -1;
	objectTag		= 0;
	isActive		= true;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
}

NCL::CSC8503::GameObject::~GameObject()	{
	delete renderObject;
	delete networkObject;
}