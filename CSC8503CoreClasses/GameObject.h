#pragma once
#include "RenderObject.h"
using std::vector;

namespace reactphysics3d {
	class RigidBody;
	class Transform;
	class CollisionCallback;
}
namespace NCL::Maths {
	class Vector3;
}

namespace NCL::CSC8503 {
	class NetworkObject;
	class RenderObject;
	class GameWorld;

	class GameObjectListener : public reactphysics3d::EventListener {
	public:
		GameObjectListener(GameWorld* world) { this->world = world; }
		~GameObjectListener(){}

		virtual void onContact(const CollisionCallback::CallbackData& callbackData) override;
	private:
		GameWorld* world;
	};

	class GameObject{
	public:
		GameObject(GameWorld* world, std::string name = "");
		virtual ~GameObject();
		virtual void Update(float dt){};
		void setActive(bool active){
			isActive = active;
	/*		if (!active) {
				physicsObject->setType(rp3d::BodyType::STATIC);
				physicsObject->setTransform(rp3d::Transform(rp3d::Vector3(0, -100, 0), rp3d::Quaternion::identity()));
			}
			else {
				physicsObject->setType(rp3d::BodyType::DYNAMIC);
				physicsObject->setTransform(rp3d::Transform(rp3d::Vector3(40, 3, 20), rp3d::Quaternion::identity()));
			}*/
		}
		bool IsActive() const {
			return isActive;
		}
		void SetTag(int tag)  {
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

		void SetYaw(reactphysics3d::Quaternion yaw) {
			this->yaw = yaw;
		}
		reactphysics3d::Quaternion GetYaw() {
			return yaw;
		}
		reactphysics3d::Quaternion GetPitch() {
			return pitch;
		}
		void SetPitch(reactphysics3d::Quaternion pitch) {
			this->pitch = pitch;
		}
		void setGrounded(bool groundedness) {
			isGrounded = groundedness;
		}
		bool IsGrounded() const {
			return isGrounded;
		}

		NCL::Maths::Vector3 collisionPoint;
		bool directionInput;

		virtual void paintHit(NCL::Maths::Vector3 position, char paintColour) {
		}

	protected:
		reactphysics3d::RigidBody*		physicsObject;
		RenderObject*		renderObject;
		NetworkObject*		networkObject;
		GameObject*			associated;
		bool		isActive;
		int			worldID;
		int			objectTag;
		std::string	name;
		reactphysics3d::Quaternion yaw;
		reactphysics3d::Quaternion pitch;
		bool isGrounded = false;
		GameWorld* world;

	//	std::string name;
	};

	class FurnitureObject : public GameObject {
	public:
		FurnitureObject(GameWorld* world, std::string name = "");
		~FurnitureObject() {}

		void paintHit(NCL::Maths::Vector3 position, char paintColour) override {
			//std::cout << "furniture hit check\n";
			switch (paintColour) {
			case 'r':
				renderObject->SetColour(NCL::Maths::Vector4(1, 0, 0, 1));
				break;
			case 'b':
				renderObject->SetColour(NCL::Maths::Vector4(0, 0, 1, 1));
				break;
			}
		}
	};
}

