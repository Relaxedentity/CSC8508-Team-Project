#include <reactphysics3d/body/RigidBody.h>
#include "GameObject.h"
#include "CollisionDetection.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Debug.h"
#include "GameWorld.h"
#include "Projectile.h"
#include "Vector3.h"

using namespace reactphysics3d;
using namespace NCL::CSC8503;

Projectile::Projectile(GameWorld* world, std::string name) :GameObject(world,name) {

}

Projectile::~Projectile() {

}

void Projectile::OnCollisionBegin(GameObject* otherObject) {

	std::cout << collisionPoint << std::endl;
	world->painted.push_back(collisionPoint);

	std::cout << "projectile paint colour: " << paintColour << "\n";

	world->testPaintNodes(collisionPoint, paintColour);

	reactphysics3d::Transform temp(reactphysics3d::Vector3(0,-50,0),reactphysics3d::Quaternion::identity());
	GetPhysicsObject()->setTransform(temp);
	GetPhysicsObject()->setType(reactphysics3d::BodyType::STATIC);

}
void Projectile::Update(float dt) {
	time -= dt;
	if (time <= 0) {
		//world->RemoveGameObject(this);
	}
}