#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "NavigationGrid.h"
#include "StateGameObject.h"
#include "BTreeObject.h"

namespace reactphysics3d {
	class PhysicsCommon;
	class PhysicsWorld;
}

namespace NCL {
	namespace CSC8503 {
		class GameObject;
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
			reactphysics3d::PhysicsWorld* GetPhysicsWorld() {
				return physicsWorld;
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
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const reactphysics3d::Vector3& cubeHalfextents);
			void BridgeConstraintTest(Vector3 pos);
			void InitDefaultFloor();
			void patrolMovement();
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void MovePlayer(GameObject* player, float dt);
			void TestPathfinding(Vector3 pos);
			void TestHedgefinding(Vector3 pos);
			BTreeObject* AddGooseToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, vector<Vector3> testNodes);
			StateGameObject* AddStateObjectToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, vector<Vector3> testNodes);
			StateGameObject* testStateObject;
			BTreeObject* goose;

			GameObject* AddFloorToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents);
			GameObject* AddSphereToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, float mass = 0.1f);
			GameObject* AddBreakableToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, float mass = 0.1f);
			GameObject* AddCubeToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents, float mass = 0.1f);
			GameObject* AddGWBlocksToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents);
			GameObject* AddButtonToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float mass = 0.1f);
			void buildGameworld();
			GameObject* AddPlayerToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			GameObject* AddPlayer2ToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			GameObject* AddEnemyToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			GameObject* AddBonusToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			GameObject* AddEmitterToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			void AddHedgeMazeToWorld();


#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			reactphysics3d::PhysicsCommon physics;
			reactphysics3d::PhysicsWorld* physicsWorld;
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
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;


			// Third Person Camera Tests

			Vector3 lockedOffset = Vector3(0, 14, 20);
			float orbitScalar = 30.0f;
			float orbitScalarMax = 30.0f;
			float orbitScalarMin = 2.0f;

			float thirdPersonYScalar = 1;
			float thirdPersonXScalar = 1.25;
			float thirdPersonZScalar = 4;

			bool thirdPerson = true;

			Vector3 orbitCameraProcess(Vector3 objPos);
			Vector3 thirdPersonCameraProcess(Vector3 objPos);
			void cameraInterpolation(Vector3 target, float dt);
			float cameraInterpBaseSpeed = 0.5f;

			Quaternion thirdPersonRotationCalc(GameWorld* world, GameObject* object, Camera* cam, Vector3 camPos);
		};
	}
}

