#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"
#include "NavigationGrid.h"
#include "StateGameObject.h"
#include "BTreeObject.h"

namespace NCL {
	namespace CSC8503 {
		class TutorialGame		{
		public:
			void InitWorld();
			TutorialGame();
			~TutorialGame();
			GameObject* getPlayer() {
				return player;
			}
			GameObject* getPlayer2() {
				return player2;
			}
			virtual void UpdateGame(float dt);
			GameObject* player;
			GameObject* emitter;
			GameObject* player2;
			GameWorld* GetGameWorld() {
				return world;
			}
			
		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitGameExamples();
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void BridgeConstraintTest(Vector3 pos);
			void InitDefaultFloor();
			void patrolMovement();
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void movePlayer(GameObject* player);
			void TestPathfinding(Vector3 pos);
			void TestHedgefinding(Vector3 pos);
			BTreeObject* AddGooseToWorld(const Vector3& position, vector <Vector3 > testNodes);
			StateGameObject* AddStateObjectToWorld(const Vector3& position, vector <Vector3 > testNodes);
			StateGameObject* testStateObject;
			BTreeObject* goose;

			GameObject* AddFloorToWorld(const Vector3& position, Vector3 size);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddBreakableToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddGWBlocksToWorld(const Vector3& position, Vector3 dimensions);
			GameObject* AddButtonToWorld(const Vector3& position, float inverseMass = 10.0f);
			void buildGameworld();
			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddPlayer2ToWorld(const Vector3& position);
			GameObject* AddEmitterToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);
			void AddHedgeMazeToWorld();


#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;
			GameObject* patrol;
			
			GameObject* button;
			GameObject* door;
			bool useGravity;
			bool inSelectionMode;
			NavigationGrid* worldGrid;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh	= nullptr;
			MeshGeometry*	sphereMesh	= nullptr;
			MeshGeometry*   gooseMesh   = nullptr;

			TextureBase*	basicTex	= nullptr;
			ShaderBase*		basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry*	charMesh	= nullptr;
			MeshGeometry*	enemyMesh	= nullptr;
			MeshGeometry*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;
		};
	}
}

