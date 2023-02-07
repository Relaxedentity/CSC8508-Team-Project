#pragma once
using std::vector;

namespace reactphysics3d {
	class RigidBody;
	class Transform;
}

namespace NCL::CSC8503 {
	class NetworkObject;
	class RenderObject;

	class GameObject	{
	public:
		GameObject(std::string name = "");
		~GameObject();

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

		RenderObject* GetRenderObject() const {
			return renderObject;
		}

		reactphysics3d::RigidBody* GetPhysicsObject() const{
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

		void SetPhysicsObject(reactphysics3d::RigidBody* newObject) {
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

		int getScore() {
			return score;
		}
		void setScore(int point) {
			score = point;
		}

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
		reactphysics3d::RigidBody*		physicsObject;
		RenderObject*		renderObject;
		NetworkObject*		networkObject;
		GameObject*			associated;
		int score;
		bool		isActive;
		int			worldID;
		int			objectTag;
		std::string	name;
	};
}

