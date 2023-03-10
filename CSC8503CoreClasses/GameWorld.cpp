#include "GameWorld.h"
#include <reactphysics3d/reactphysics3d.h>
#include "GameObject.h"
#include "CollisionDetection.h"
#include "Camera.h"
#include "Debug.h"
#include "RenderObject.h"

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
	secCamera = new Camera();

	shuffleObjects		= false;
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

GameWorld::~GameWorld()	{
	gameObjects.erase(gameObjects.begin(), gameObjects.end());
}

void GameWorld::Clear() {
	gameObjects.erase(gameObjects.begin(), gameObjects.end());
	worldIDCounter		= 0;
	worldStateCounter	= 0;
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		i->GetPhysicsObject()->removeCollider(0);
		physicsWorld->destroyRigidBody(i->GetPhysicsObject());
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
		o->GetPhysicsObject()->removeCollider(0);
		physicsWorld->destroyRigidBody(o->GetPhysicsObject());
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

void GameWorld::AddPaintNode(PaintNode* o) {
	paintNodes.emplace_back(o);
}

void GameWorld::RemovePaintNode(PaintNode* o, bool andDelete) {
	paintNodes.erase(std::remove(paintNodes.begin(), paintNodes.end(), o), paintNodes.end());
	if (andDelete) {
		delete o;
	}
}

void GameWorld::paintTally() {
	int totalNodes = 0;
	int colourCountOne = 0;
	int colourCountTwo = 0;
	for (auto& i : paintNodes) {
		totalNodes++;
		if (i->getColour() == 'r') {
			colourCountOne++;
		}
		else if (i->getColour() == 'b') {
			colourCountTwo++;
		}
	}
	//std::cout << "Total nodes: " << totalNodes << "\n";

	//std::cout << "colour count 1: " << colourCountOne << "\n";
	colourOneScore = (float)colourCountOne / totalNodes;
	//std::cout << "colour score 1: " << colourOneScore << "\n";

	//std::cout << "colour count 2: " << colourCountTwo << "\n";
	colourTwoScore = (float)colourCountTwo / totalNodes;
	//std::cout << "colour score 2: " << colourTwoScore << "\n";
}

void GameWorld::testPaintNodes(Vector3 inPos, char iChar) {
	bool hasChanged = false;
	for (auto& i : paintNodes) {
		if ( (inPos-i->getPos()).Length() < 2.5 && i->getColour() != iChar ) {
			hasChanged = true;
			//std::cout << "we got a hit!\n";
			i->setColour(iChar);
			//std::cout << "node colour: " << i->getColour() << "\n";
		}
	}
	if (hasChanged) paintTally();
}

void GameWorld::drawPaintNodes() {
	for (auto& i : paintNodes) {
		Vector4 paintColour;

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


void GameWorld::AddPaintBall() {
	paintBallAmount += 1;
}

void GameWorld::RemovePaintBall() {
	paintBallAmount -= 1;
}

int GameWorld::GetPaintBalls() {
	return paintBallAmount;
}

void GameWorld::AddMapNode(MapNode* o) {
	mapNodes.emplace_back(o);
}

void GameWorld::RemoveMapNode(MapNode* o, bool andDelete) {
	mapNodes.erase(std::remove(mapNodes.begin(), mapNodes.end(), o), mapNodes.end());
	if (andDelete) {
		delete o;
	}
}

void GameWorld::paintSphereTest(GameObject* inputObject, Vector3 position, char paintColour) {
	reactphysics3d::Transform temp = reactphysics3d::Transform(reactphysics3d::Vector3(position.x, position.y, position.z), reactphysics3d::Quaternion::identity());
	paintOrb->GetPhysicsObject()->setTransform(temp);
	paintOrb->GetPhysicsObject()->setType(reactphysics3d::BodyType::DYNAMIC);
	
	for (auto& i : gameObjects) {
		if (inputObject == i) continue;
		if (physicsWorld->testOverlap(paintOrb->GetPhysicsObject(), i->GetPhysicsObject())) {
			i->GetRenderObject()->PaintSpray(position, paintColour);
		}
	}
	
	reactphysics3d::Transform dumpster = reactphysics3d::Transform(reactphysics3d::Vector3(0, -100, 0), reactphysics3d::Quaternion::identity());
	paintOrb->GetPhysicsObject()->setTransform(dumpster);
	paintOrb->GetPhysicsObject()->setType(reactphysics3d::BodyType::STATIC);
}