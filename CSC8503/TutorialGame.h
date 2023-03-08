#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "NavigationGrid.h"
#include "StateGameObject.h"
#include "BTreeObject.h"

#include "PlayerObject.h"

#include "Sound.h"

namespace reactphysics3d {
	class PhysicsCommon;
	class PhysicsWorld;
}

namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class Projectile;
		class TutorialGame		{
		public:
			void InitWorld();
			TutorialGame();
			~TutorialGame();
			PlayerObject* getPlayer() {
				return player;
			}
			PlayerObject* getPlayer2() {
				return player2;
			}
			virtual void UpdateGame(float dt);
			
			PlayerObject* player;
			PlayerObject* playerCoop;
			GameObject* emitter;
			
			PlayerObject* player2;
			PlayerObject* player3;
			PlayerObject* player4;

			SoundObject* firevoice;//
			SoundObject* movevoice;//
			SoundObject* jumpvoice;//
			SoundObject* Init;//
			float timedetection = 0.0;//
			float timedetection2 = 0.0;//

			GameWorld* GetGameWorld() {
				return world;
			}

			reactphysics3d::PhysicsWorld* GetPhysicsWorld() {
				return physicsWorld;
			}
		    
		protected:
			void InitialiseAssets();
			void InitSound();
			void InitCamera();
			void InitCameraSec();
			void UpdateKeys();

			
			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitGameExamples();
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const reactphysics3d::Vector3& cubeHalfextents);
			void InitDefaultFloor();
			void InitProjectiles();
			void InitPaintOrb();
			
			bool SelectObject();

			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void MovePlayer(PlayerObject* player, float dt);

			//test
			void MovePlayerCoop(PlayerObject* player, float dt);
			void FirstController(GameObject& player);
			void SecondController(GameObject& player);

			void TestPathfinding(Vector3 pos);
			void TestHedgefinding(Vector3 pos);
			BTreeObject* AddGooseToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, vector<Vector3> testNodes);
			StateGameObject* AddStateObjectToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, vector<Vector3> testNodes);
			StateGameObject* testStateObject;
			BTreeObject* goose;

			GameObject* AddFloorToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents);
			Projectile* AddProjectileToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, char colour, float mass = 0.1f);
			GameObject* AddBreakableToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, float mass = 0.1f);
			GameObject* AddCubeToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents, float mass = 0.1f);
			GameObject* AddGWBlocksToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents);
			GameObject* AddButtonToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float mass = 0.1f);
			void buildGameworld();
			PlayerObject* AddPlayerToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, int netID, int worldID);
			GameObject* AddEnemyToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			PlayerObject* AddPlayerForCoop(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);

			GameObject* AddBonusToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			GameObject* AddEmitterToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			void AddHedgeMazeToWorld();


			// Making Rebellion mesh-based objects
			GameObject* AddRebWallMainToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale);
			GameObject* AddRebWallRightToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale, bool nodes);
			GameObject* AddRebWallLeftToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale, bool nodes);

			void AddRebWallSquareToWorld(const reactphysics3d::Vector3& position);

			void AddRebWallNorthToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallSouthToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallEastToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallWestToWorld(const reactphysics3d::Vector3& position);

			void AddRebWallDualVerticalToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallDualHorizontalToWorld(const reactphysics3d::Vector3& position);

			void AddRebWallOpeningNorthToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallOpeningSouthToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallOpeningEastToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallOpeningWestToWorld(const reactphysics3d::Vector3& position);

			void AddRebWallCornerNorthEastToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallCornerNorthWestToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallCornerSouthEastToWorld(const reactphysics3d::Vector3& position);
			void AddRebWallCornerSouthWestToWorld(const reactphysics3d::Vector3& position);

			void RenderDebug(float dt);

			/*sound functions*/
			void MainScreenFireMapping(Vector3 sphereintipos);
			void MainScreenMoveMapping(Vector3 playermoveposition, bool directionInput);
			void MainScreenJumpMapping(Vector3 sphereintipos);

			void SecScreenFireMapping(Vector3 sphereintipos);
			void SecScreenMoveMapping(Vector3 playermoveposition, bool directionInput);
			void SecScreenJumpMapping(Vector3 sphereintipos);

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
			GameObject* floor;

			bool initSplitScreen;
			bool coopMode;

			bool useGravity;
			bool inSelectionMode;
			bool freeCamera;
			bool mouseLock;
			bool debug;
			NavigationGrid* worldGrid;

			float		forceMagnitude;

			float		health =0.8f;
			float		secHealth = 0.2f;
			float		timeLimit;

			GameObject* selectionObject = nullptr;
			GameObject* selectionObjectSec = nullptr;
			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh	= nullptr;
			MeshGeometry*	sphereMesh	= nullptr;
			MeshGeometry*   gooseMesh   = nullptr;

			TextureBase*	basicTex	= nullptr;
			ShaderBase*		basicShader = nullptr;
			ShaderBase*		charShader	= nullptr;

			//Coursework Meshes
			MeshGeometry*	charMesh	= nullptr;
			MeshGeometry*	enemyMesh	= nullptr;
			MeshGeometry*	bonusMesh	= nullptr;

			// Rebellion Assets
			TextureBase*	chairTex	= nullptr;
			MeshGeometry*	chairMesh	= nullptr;

			TextureBase*	corridorTexture			= nullptr;
			MeshGeometry*	corridorStraightMesh	= nullptr;
			MeshGeometry*	corridorCornerRightSideMesh			= nullptr;
			MeshGeometry*	corridorCornerLeftSideMesh			= nullptr;

			// Test Mesh for quick changing
			MeshGeometry*	testMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			GameObject* lockedSecObject = nullptr;

			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}
			void LockCameraToObject2(GameObject* o) {
				lockedSecObject = o;
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

			Projectile* projectiles[100];
			int currentProjectile = 0;

			bool thirdPerson = true;

			Vector3 orbitCameraProcess(Vector3 objPos, Camera& camera, GameObject* ignorePlayer);
			Vector3 thirdPersonCameraProcess(Vector3 objPos, Camera& camera, GameObject* currPlayer);
			void cameraInterpolation(Vector3 target, float dt, Camera& camera);
			float cameraInterpBaseSpeed = 0.5f;

			Quaternion thirdPersonRotationCalc(GameWorld* world, GameObject* object, Camera* cam, Vector3 camPos);

			// Paint Node Functions
			void addPaintNodeToWorld(Vector3 location);

			void addMapNodeToWorld(Vector3 location);
			// Rendering the character's paint track
			void PlayerPaintTracks(PlayerObject* player, char paintColour);
		};
	}
}