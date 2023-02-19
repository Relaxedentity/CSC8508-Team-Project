#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

using std::vector;

namespace NCL::CSC8503 {
	class NetworkObject;
	class RenderObject;
	class PhysicsObject;

	class GameObject	{
	public:
		GameObject(std::string name = "");
		~GameObject();

		void SetBoundingVolume(CollisionVolume* vol) {
			boundingVolume = vol;
		}

		const CollisionVolume* GetBoundingVolume() const {
			return boundingVolume;
		}
		void setActive(bool active){
			isActive = active;
		}
		bool IsActive() const {
			return isActive;
		}
		void SetTag(int tag) {
			objectTag = tag;
		}
		int GetTag() {
			return objectTag;
		}
		Transform& GetTransform() {
			return transform;
		}

		RenderObject* GetRenderObject() const {
			return renderObject;
		}

		PhysicsObject* GetPhysicsObject() const {
			return physicsObject;
		}

		NetworkObject* GetNetworkObject() const {
			return networkObject;
		}

		void SetNetworkObject(NetworkObject* n) {
			networkObject = n;
		}

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		const std::string& GetName() const {
			return name;
		}

		virtual void OnCollisionBegin(GameObject* otherObject) {
			//std::cout << "OnCollisionBegin event occured!\n";
		}

		virtual void OnCollisionEnd(GameObject* otherObject) {
			//std::cout << "OnCollisionEnd event occured!\n";
		}

		bool GetBroadphaseAABB(Vector3& outsize) const;
		int getScore() {
			return score;
		}
		void setScore(int point) {
			score = point;
		}
		void UpdateBroadphaseAABB();

		void SetWorldID(int newID) {
			worldID = newID;
		}
		void SetAssociated(GameObject* asso) {
			associated = asso;
		}
		GameObject* GetAssociated() {
			return associated;
		}
		int		GetWorldID() const {
			return worldID;
		}


	protected:
		Transform			transform;

		CollisionVolume*	boundingVolume;
		PhysicsObject*		physicsObject;
		RenderObject*		renderObject;
		NetworkObject*		networkObject;
		GameObject*			associated;
		int score;
		bool		isActive;
		int			worldID;
		int			objectTag;
		std::string	name;

		Vector3 broadphaseAABB;
	};
}

