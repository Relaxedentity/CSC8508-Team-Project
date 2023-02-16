#include <reactphysics3d/body/RigidBody.h>
#include "GameObject.h"
#include "CollisionDetection.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Debug.h"
#include "GameWorld.h"

using namespace reactphysics3d;
using namespace NCL::CSC8503;

void GameObjectListener::onContact(const CollisionCallback::CallbackData& callbackData) {
	for (uint p = 0; p < callbackData.getNbContactPairs(); p++) {
		CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);
		GameObject* body1 = nullptr;
		GameObject* body2 = nullptr;
		world->OperateOnContents(
			[&](GameObject* o) {
				if (o->GetPhysicsObject() == contactPair.getBody1()) {
					body1 = o;
				}
				else if (o->GetPhysicsObject() == contactPair.getBody2()) {
					body2 = o;
				}
			}
		);
		
		std::vector<CollisionCallback::ContactPoint> contactPoints;
		for (uint c = 0; c < contactPair.getNbContactPoints(); c++) {
			contactPoints.push_back(contactPair.getContactPoint(c));
			
			CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(c);
			reactphysics3d::Vector3 worldPoint = contactPair.getCollider1()->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider1();
			
			//std::cout << worldPoint.to_string();
			Vector3 ncl = Vector3(worldPoint);
			body1->collisionPoint = worldPoint;
			body2->collisionPoint = worldPoint;
			//Debug::DrawLine(Vector3(ncl.x, ncl.y - 3, ncl.z), Vector3(ncl.x, ncl.y + 3, ncl.z), Vector4(1, 0, 0, 1), 2);
			//Debug::DrawLine(Vector3(ncl.x - 3, ncl.y, ncl.z), Vector3(ncl.x + 3, ncl.y, ncl.z), Vector4(1, 0.5f, 0, 1), 2);
			//Debug::DrawLine(Vector3(ncl.x, ncl.y, ncl.z - 3), Vector3(ncl.x, ncl.y, ncl.z + 3), Vector4(1, 0, 0.5f, 1), 2);
		}
		
		switch (contactPair.getEventType()) {
		case CollisionCallback::ContactPair::EventType::ContactStart:
			body1->OnCollisionBegin(body2);
			body2->OnCollisionBegin(body1);
			break;
		case CollisionCallback::ContactPair::EventType::ContactExit:
			body1->OnCollisionEnd(body2);
			body2->OnCollisionEnd(body1);
			break;
		}
	}
}

NCL::CSC8503::GameObject::GameObject(GameWorld* world, string objectName)	{
	name			= objectName;
	worldID			= -1;
	objectTag		= 0;
	isActive		= true;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
	this->world = world;

}

NCL::CSC8503::GameObject::~GameObject()	{
	delete renderObject;
	delete networkObject;
}