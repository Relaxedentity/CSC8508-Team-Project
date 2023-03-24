#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"

#include "BehaviourSequence.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourAction.h"
#include "MeshAnimation.h"
#include "TerrainObject.h"


namespace NCL {
	namespace CSC8503 {
		class BasicAI : public GameObject {
		public:
			BasicAI(GameWorld* world, std::vector<NCL::Maths::Vector3> mapNodes, std::string name);
			~BasicAI();

			void UpdateBoss(float dt, NCL::Maths::Vector3& playePos);
			void Update(float dt) override;

			void CreateBehaviourTree();
			bool SeenPlayer();

			void OnCollisionBegin(GameObject* otherObject) override;

			void DrawWedgeVolume(float height, float AngThres, float outerRadius, float innerRadius);
			void CreatePath(NCL::Maths::Vector3& position);
			void DisplayPath();
			void WalkPath(NCL::Maths::Vector3& destination);

			float VectorMagnitude(float x, float y, float z);
			double DegreesToRadian(double degrees);

			void SetRotationToPlayer();
			void UpdateAnim(BasicAI* p, MeshAnimation* anim, float& ftime, int& cframe);
			void DrawAnim(BasicAI* p, MeshAnimation* anim, int& cframe);

			MeshAnimation* BlendAnimation(MeshAnimation* anim1, MeshAnimation* anim2, float blendFactor);
		
		protected:
			const float PI = 3.14159265359f;

			int range;
			float timeLimit;
			std::string	name;
			float aiHealth = 1;

			std::vector<NCL::Maths::Vector3> pathNodes;
			int currentNode;

			MeshAnimation* aiWalkAnim = nullptr;
			MeshAnimation* aiRunAnim = nullptr;
			MeshAnimation* aicloseAttackAnim = nullptr;
			MeshAnimation* aiJumpBack = nullptr;
			MeshAnimation* aiFarAttackTwoAnim = nullptr;
			MeshAnimation* aiFarAttackAnim = nullptr;
			MeshAnimation* aiRightStrafeAnim = nullptr;
			MeshAnimation* aiLeftStrafeAnim = nullptr;
			MeshAnimation* aiDamaged = nullptr;

			MeshAnimation* blendedAnimm = nullptr;

			NCL::Maths::Vector3 currPlayerPos;

			NCL::Maths::Vector3 dest1;
			NCL::Maths::Vector3 dest2;
			NCL::Maths::Vector3 dest3;
			NCL::Maths::Vector3 dest4;

		
			float hitTime;
			bool destNotArrived;
			bool seenPlayer;
			bool foundPath;
			bool walkToPlayer;
			bool inTime;

			bool walkOrAttack;

			int nodeIndex;
			int rNum;
			int moveDirection = 1;
			float movSpeed = 0;

			//anim
			int currentFrame;
			float frameTime;

			float height;
			float AngThres;
			float outerRadius;
			float innerRadius;
			float attackTime;

			int attackSelect;

			// health
			float health;

			float strafeTime = 3;
			const float minDist = 7.0f;
			const float maxDist = 15.0f;
			const float strafeDist = 10.0f;

			BehaviourSequence* rootSequence;
			BehaviourSequence* patrolSequence;
			BehaviourSequence* attackSequence;
			BehaviourSelector* rangeForAttackSelector;
			BehaviourSelector* moveSelector;
			BehaviourSequence* strafeBehaviour;
			BehaviourSequence* runThenAttackSequence;
			BehaviourState currentstate;
			BehaviourSequence* jumpSequence;

		};
	}
}
