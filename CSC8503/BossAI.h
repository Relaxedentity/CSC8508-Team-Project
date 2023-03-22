#pragma once
#include "GameObject.h"
#include "GameTechRenderer.h"

#include "BehaviourSequence.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourAction.h"


enum AttackRange {
	closeRange,
	midRange,
	farRange,
};

namespace NCL {
	namespace CSC8503 {
		//class StateMachine;
		class BossAI : public GameObject {
		public:
			BossAI(GameWorld* world, std::vector<NCL::Maths::Vector3> mapNodes);
			~BossAI();

			void CreateBehaviourTree();

			void UpdateBoss(float dt, NCL::Maths::Vector3& playePos) ;


			void Update(float dt) override;
			NCL::Maths::Vector3 getTargerPosition() {
				return targetPosition;
			};
			void setTargetPosition(NCL::Maths::Vector3 position) {
				targetPosition = position;
			};
			GameObject* getTarget1() {
				return target1;
			};
			GameObject* getTarget2() {
				return target2;
			};
			void setTarget1(GameObject* gametarget) {
				target1 = gametarget;
			};
			void setTarget2(GameObject* gametarget) {
				target2 = gametarget;
			};
			
			bool SeenPlayer();

			void DrawWedgeVolume( float height, float AngThres, float outerRadius, float innerRadius);
			void CreatePath(NCL::Maths::Vector3& position);
			void DisplayPath();
			void WalkPath(NCL::Maths::Vector3& destination);

			float VectorMagnitude(float x, float y, float z);
			double DegreesToRadian(double degrees);

			void SetRotationToPlayer();


		protected:
			const float PI = 3.14159265359f;

			uint8_t range;
			float timeLimit;

			GameObject* target1;
			GameObject* target2;

			NCL::Maths::Vector3 targetPosition;
			NCL::Maths::Vector3 targetDestination;

			std::vector<NCL::Maths::Vector3> pathNodes;
			int currentNode;

			NCL::Maths::Vector3 dest;
			NCL::Maths::Vector3 currPlayerPos;
			
			NCL::Maths::Vector3 dest1;
			NCL::Maths::Vector3 dest2;
			NCL::Maths::Vector3 dest3;
			NCL::Maths::Vector3 dest4;


			bool destNotArrived;
			bool seenPlayer;
			bool foundPath;
			bool walkToPlayer;
			bool inTime;

			int nodeIndex;
			int rNum;
			int moveDirection = 1;
			float movSpeed = 0;

			//const float initialSpeed = 100.0f;  // Initial movement speed
			// float minSpeed = 10.0f;  // Minimum movement speed (when close to the player)
			// const float stopDist = 10.0f;  // Distance at which the AI stops moving

			float height;
			float AngThres;
			float outerRadius;
			float innerRadius;

			float strafeTime = 3;
			const float minDist = 7.0f;
			const float maxDist = 15.0f;
			const float strafeDist = 10.0f;

			BehaviourSequence* rootSequence;

			BehaviourSequence* patrolSequence;
			BehaviourSequence* attackSequence;

			BehaviourSelector* rangeForAttackSelector;
			BehaviourSelector* moveSelector;

			BehaviourState currentstate;
		};
	}
}