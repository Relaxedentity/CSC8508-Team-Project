#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "NavigationGrid.h"
#include "StateGameObject.h"
#include "BTreeObject.h"
#include "MeshMaterial.h"
#include "PlayerObject.h"
#include "MeshAnimation.h"
#include "Sound.h"

namespace reactphysics3d {
	class PhysicsCommon;
	class PhysicsWorld;
}

namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class TerrainObject;
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

			SoundObject* voice;//
			SoundObject* Init; //
			ISoundEngine* initV;
			float timedetection = 0.0;//
			float timedetection2 = 0.0;//

			GameWorld* GetGameWorld() {
				return world;
			}

			reactphysics3d::PhysicsWorld* GetPhysicsWorld() {
				return physicsWorld;
			}
			GameObject* AddEmitterToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation,char colour);

			float redScore = 0.0f;
			float blueScore = 0.0f;
			
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
			void InitProjectiles(ShaderBase* shader);
			void InitPaintOrb();
			
			bool SelectObject();

			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void MovePlayer(PlayerObject* player, float dt, Vector3 camPos);

			//test
			void MovePlayerCoop(PlayerObject* player, float dt);


			void TestPathfinding(Vector3 pos);
			void TestHedgefinding(Vector3 pos);
			BTreeObject* AddGooseToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, vector<Vector3> testNodes);
			StateGameObject* AddStateObjectToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, vector<Vector3> testNodes);
			StateGameObject* testStateObject;
			BTreeObject* goose;

			// animation 
			void DrawAnim(PlayerObject* p, MeshAnimation* anim);
			void UpdateAnim(PlayerObject* p, MeshAnimation* anim);

			TerrainObject* AddFloorToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents, MapNode* node);
			Projectile* AddProjectileToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, char colour, float mass = 0.1f);
			GameObject* AddBreakableToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, float mass = 0.1f);
			GameObject* AddCubeToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents, float mass = 0.1f);
			GameObject* AddGWBlocksToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents);
			GameObject* AddButtonToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float mass = 0.1f);
			void buildGameworld();
			PlayerObject* AddPlayerToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, ShaderBase* shader, char paintColour, int netID, int worldID);
			GameObject* AddEnemyToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);

			//Power-up items
			GameObject* AddBonusToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			GameObject* AddCapsuleToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			GameObject* AddCatToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation);
			
			void AddHedgeMazeToWorld();
			
			// Making Rebellion mesh-based objects
			TerrainObject* AddRebWallMainToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale);
			TerrainObject* AddRebWallRightToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale, bool nodes);
			TerrainObject* AddRebWallLeftToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale, bool nodes);

			void AddRebWallSquareToWorld(const reactphysics3d::Vector3& position, MapNode* node);

			void AddRebWallNorthToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallSouthToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallEastToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallWestToWorld(const reactphysics3d::Vector3& position, MapNode* node);

			void AddRebWallDualVerticalToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallDualHorizontalToWorld(const reactphysics3d::Vector3& position, MapNode* node);

			void AddRebWallOpeningNorthToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallOpeningSouthToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallOpeningEastToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallOpeningWestToWorld(const reactphysics3d::Vector3& position, MapNode* node);

			void AddRebWallCornerNorthEastToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallCornerNorthWestToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallCornerSouthEastToWorld(const reactphysics3d::Vector3& position, MapNode* node);
			void AddRebWallCornerSouthWestToWorld(const reactphysics3d::Vector3& position, MapNode* node);

			void RenderDebug(float dt);

			void FreezingPlayers(float dt);
			void RegeneratePowerupProps(float dt);

			/*sound functions*/
			void MainScreenFireMapping(Vector3 sphereintipos);
			void MainScreenMoveMapping(Vector3 playermoveposition, bool directionInput);
			void MainScreenJumpMapping(Vector3 sphereintipos);

			void SecScreenFireMapping(Vector3 sphereintipos);
			void SecScreenMoveMapping(Vector3 playermoveposition, bool directionInput);
			void SecScreenJumpMapping(Vector3 sphereintipos);
			void shootPaint(PlayerObject* p, float dt, Camera* c);
			Vector3 MoveForward(PlayerObject* p, Quaternion Yaw, Vector3 endVelocity);
			Vector3 MoveBackward(PlayerObject* p, Quaternion Yaw, Vector3 endVelocity);
			Vector3 MoveLeft(PlayerObject* p, Quaternion Yaw, Vector3 endVelocity);
			Vector3 MoveRight(PlayerObject* p, Quaternion Yaw, Vector3 endVelocity);
			void updateCamera(PlayerObject* player, float dt);
			void ShootProjectile(PlayerObject* p, Quaternion Pitch);
			void moveDesignatedPlayer(PlayerObject* p, float dt, Vector3 camPos);

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

			bool initSplitScreen;
			bool coopMode;
			bool gpConnected;

			bool useGravity;
			bool inSelectionMode;
			bool freeCamera;
			bool mouseLock;
			bool debug;
			bool isMultiplayer;
			float renderTime = 0;
			float accumulator = 0;
			const float timeStep = 1.0f / 60.0f;

			NavigationGrid* worldGrid;

			float		forceMagnitude;

			float		health =0.8f;
			float		secHealth = 0.2f;
			float		timeLimit;

			float       normalRate = 0.1f;
			float       shotGunRate = 0.5f;
			float       invokeTime = normalRate;
			float       invokeTime2 = normalRate;

			float     p1pauseTime = 3.0f;
			float     p2pauseTime = 3.0f;
			float    shotGunModeTime = 10.0f;



			GameObject* selectionObject = nullptr;
			GameObject* selectionObjectSec = nullptr;
			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh	= nullptr;
			MeshGeometry*	sphereMesh	= nullptr;
			MeshGeometry*   gooseMesh   = nullptr;
			MeshGeometry* playerMesh = nullptr;
			MeshAnimation* playerWalkAnim = nullptr;
			MeshAnimation* playerIdleAnim = nullptr;
			TextureBase*	basicTex	= nullptr;
			TextureBase* terrainTex = nullptr;////////////
			ShaderBase*		basicShader = nullptr;
			OGLShader* animatedShader = nullptr;
			OGLShader* animatedShaderA = nullptr;
			ShaderBase*		charShader	= nullptr;
			MeshMaterial* playerMat = nullptr;
			vector <GLuint > playerTextures;
			//Coursework Meshes
			MeshGeometry*	charMesh	= nullptr;
			MeshGeometry*	enemyMesh	= nullptr;
			MeshGeometry*	bonusMesh	= nullptr;
			// Rebellion Assets
			TextureBase*	chairTex	= nullptr;
			MeshGeometry*	chairMesh	= nullptr;
			TextureBase* playerTex = nullptr;
			TextureBase*	corridorTexture			= nullptr;
			MeshGeometry*	corridorStraightMesh	= nullptr;
			MeshGeometry*	corridorCornerRightSideMesh			= nullptr;
			MeshGeometry*	corridorCornerLeftSideMesh			= nullptr;

			///Particle
			OGLMesh* pointSprites = nullptr;
			OGLShader* particleShader = nullptr;
			TextureBase* particleTex = nullptr;
			//Power-up Items
			ShaderBase* itemShader = nullptr;
			TextureBase* capsuleTex = nullptr;
			MeshGeometry* catMesh = nullptr;
			TextureBase* catTex = nullptr;

			GameObject* cat = nullptr;
			GameObject* coin = nullptr;
			GameObject* coin2 = nullptr;
			GameObject* capsule = nullptr;
			GameObject* capsule2 = nullptr;
			
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
			float thirdPersonXScalar = 1.5;
			float thirdPersonZScalar = 5;
			Projectile* projectiles[100];
			Projectile* oneShot[5];
			int currentProjectile = 0;

			bool thirdPerson = true;

			Vector3 orbitCameraProcess(Vector3 objPos, Camera& camera, GameObject* ignorePlayer);
		
			Vector3 thirdPersonCameraProcess(Vector3 objPos, Camera& camera, GameObject* currPlayer);
			void cameraInterpolation(Vector3 target, float dt, Camera& camera);
			float cameraInterpBaseSpeed = 0.5f;

			Quaternion thirdPersonRotationCalc(GameWorld* world, GameObject* object, Camera* cam, Vector3 camPos);

			vector<reactphysics3d::Vector3> itemPos;

			// Rendering the character's paint track
			void PlayerPaintTracks(PlayerObject* player, char paintColour);
		};
	}
}