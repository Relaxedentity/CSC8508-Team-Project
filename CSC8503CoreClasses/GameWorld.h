#pragma once
#include <random>

#include "Ray.h"
#include "CollisionDetection.h"

namespace reactphysics3d {
	class PhysicsWorld;
	class RaycastCallback;
}

namespace NCL {
		class Camera;
		using Maths::Ray;
	namespace CSC8503 {
		class GameObject;
		class PaintNode;
		class MapNode;

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		struct SceneContactPoint {
			bool isHit;
			float hitFraction;
			reactphysics3d::Vector3 hitPos;
			reactphysics3d::Vector3 normal;
			reactphysics3d::CollisionBody* body;
			GameObject* object;
		};

		class RaycastManager : public reactphysics3d::RaycastCallback {
		public:

			RaycastManager(){ ignoreBody = nullptr; }
			~RaycastManager(){}

			virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& raycastInfo) override;

			bool isHit() {
				return !hitPoints.empty();
			}

			void setIgnore(reactphysics3d::RigidBody* ignoreThis) {
				ignoreBody = ignoreThis;
			}

			SceneContactPoint* getHit() {
				SceneContactPoint* nearest = new SceneContactPoint();
				nearest->hitFraction = 1.1f;

				for (auto i : hitPoints) {
					if (i->hitFraction < nearest->hitFraction) {
						nearest = i;
					}
				}

				return nearest;
			}

			void clear() {
				hitPoints.clear();
				ignoreBody = nullptr;
			}
		private:
			std::vector<SceneContactPoint*> hitPoints;
			reactphysics3d::RigidBody* ignoreBody;
		};

		class GameWorld	{
		public:
			GameWorld(reactphysics3d::PhysicsWorld* physicsWorld);
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);

			Camera* GetMainCamera() const {
				return mainCamera;
			}
			Camera* GetSecCamera() const {
				return secCamera;
			}
			GameObject* GetPlayer() {
				return player;
			}

			void SetPlayer(GameObject* p) {
				player = p;
			}
			
			float GetPlayerHealth() {
				return playerHealth;
			}

			float GetPlayerCoopHealth()
			{
				return playerCoopHealth;
			}

			void SetPlayerCoop(GameObject* p) {
				playerCoop = p;
			}

			GameObject* GetPlayerCoop() {
				return playerCoop;
			}

			void SetPlayerCoopHealth(float health) {
				playerCoopHealth = health;
			}

			void SetPlayerHealth(float health) {
				playerHealth = health;
			}

			void SetCollisionListener(GameObjectListener* listener) {
				collisionManager = listener;
				physicsWorld->setEventListener(listener);
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}
			void SetObjectCount(int count) {
				objectCount = count;
			}
			int GetObjectCount() {
				return objectCount;
			}
			SceneContactPoint* Raycast(reactphysics3d::Ray& r, GameObject* ignore = nullptr) const;

			virtual void UpdateWorld(float dt);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			int GetWorldStateID() const {
				return worldStateCounter;
			}
			vector<Vector4> painted;

			void paintTally();
			void testPaintNodes(Vector3 inPos, char iChar);
			void drawPaintNodes();

			void AddPaintNode(PaintNode* o);
			void RemovePaintNode(PaintNode* o, bool andDelete);

			void AddMapNode(MapNode* o);
			void RemoveMapNode(MapNode* o, bool andDelete);

			float getColourOneScore() { return colourOneScore; }
			float getColourTwoScore() { return colourTwoScore; }

			void AddPaintBall();
			void RemovePaintBall();
			int GetPaintBalls();

		protected:
			std::vector<GameObject*> gameObjects;

			Camera* mainCamera;
			Camera* secCamera;

			
			bool shuffleConstraints;
			bool shuffleObjects;
			int		worldIDCounter;
			int		worldStateCounter;
			int objectCount;
			reactphysics3d::PhysicsWorld* physicsWorld;
			RaycastManager* raycastManager;
			GameObjectListener* collisionManager;
			GameObject* player;
			float playerHealth;

			GameObject* playerCoop;
			float playerCoopHealth;

			std::vector<PaintNode*> paintNodes;
			float colourOneScore = 0;
			float colourTwoScore = 0;

			int paintBallAmount;

			std::vector<MapNode*> mapNodes;
		};

		class PaintNode {
		public:
			PaintNode(Vector3 inPos) { pos = inPos; }
			char getColour() const { return colour; }
			Vector3 getPos() { return pos; }
			void setColour(char inColour) { colour = inColour; }
			void setPos(Vector3 inPos) { pos = inPos; }
		protected:
			Vector3 pos;
			char colour = 'n';
		};
	}
}