#include "GameWorld.h"
#include <reactphysics3d/reactphysics3d.h>
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "Camera.h"

using namespace NCL;
using namespace NCL::CSC8503;

rp3d::decimal RaycastManager::notifyRaycastHit(const rp3d::RaycastInfo& raycastInfo) {
	if (ignoreBody == raycastInfo.body) { return -1; }

	rp3d::Vector3 n = raycastInfo.worldNormal;
	rp3d::Vector3 hitPos = raycastInfo.worldPoint;

	SceneContactPoint* collision = new SceneContactPoint(); 
	collision->isHit = true;
	collision->hitFraction = raycastInfo.hitFraction;
	collision->hitPos = hitPos;
	collision->normal = n;
	collision->body = raycastInfo.body;

	hitPoints.push_back(collision);

	//Debug::DrawLine(hitPos, hitPos + n, Vector4(1, 1, 0, 1), 4.0f);
	return raycastInfo.hitFraction;
}

GameWorld::GameWorld(reactphysics3d::PhysicsWorld* physicsWorld)	{
	mainCamera = new Camera();
	this->physicsWorld = physicsWorld;
	raycastManager = new RaycastManager();

	shuffleConstraints	= false;
	shuffleObjects		= false;
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

GameWorld::~GameWorld()	{
}

void GameWorld::Clear() {
	gameObjects.clear();
	constraints.clear();
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		//physicsWorld->destroyRigidBody(o->GetPhysicsObject());
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
	worldStateCounter++;
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		//physicsWorld->destroyRigidBody(o->GetPhysicsObject());
		delete o;
	}
	worldStateCounter++;
}

void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt) {
	auto rng = std::default_random_engine{};

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);

	if (shuffleObjects) {
		std::shuffle(gameObjects.begin(), gameObjects.end(), e);
	}

	if (shuffleConstraints) {
		std::shuffle(constraints.begin(), constraints.end(), e);
	}
}

SceneContactPoint* GameWorld::Raycast(reactphysics3d::Ray& r, GameObject* ignoreThis) const {
	raycastManager->clear();
	if (ignoreThis) { raycastManager->setIgnore(ignoreThis->GetPhysicsObject()); }

	physicsWorld->raycast(r, raycastManager);
	SceneContactPoint* dummy = new SceneContactPoint();
	dummy->isHit = false;
	if (!raycastManager->isHit()) { return dummy; }

	SceneContactPoint* closestHit = raycastManager->getHit();
	rp3d::Vector3 n = closestHit->normal;
	rp3d::Vector3 hitPos = closestHit->hitPos;
	//Debug::DrawLine(hitPos, hitPos + (2 * n), Vector4(1, 0, 0, 1), 4.0f);

	for (auto& i : gameObjects) {
		if (i->GetPhysicsObject() == closestHit->body) {
			closestHit->object = i;
		}
	}

	return closestHit;
}


/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
	constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
	if (andDelete) {
		delete c;
	}
}

void GameWorld::GetConstraintIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const {
	first	= constraints.begin();
	last	= constraints.end();
}