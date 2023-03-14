#pragma once
#include "GameObject.h"

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
		class StateMachine;
		class BossAI : public GameObject {
		public:
			BossAI(GameWorld* world, std::vector<NCL::Maths::Vector3> testNodes);
			~BossAI();

			void Update(float dt) override;

			NCL::Maths::Vector3 getTargetPosition() {
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
		protected:

			void WalkPath(NCL::Maths::Vector3 position);
			bool SeenPlayer();
			void StrafeAroundPlayer(NCL::Maths::Vector3 position);

			uint8_t range;
			float timeLimit;

			GameObject* target1;
			GameObject* target2;
			NCL::Maths::Vector3 targetPosition;

			std::vector<NCL::Maths::Vector3> nodes;
			int currentNode;

			// root
			BehaviourSequence* rootSequence;

			// first layer
			BehaviourSequence* patrolSequence;
			BehaviourSequence* attackSequence;

			//second layer
			BehaviourSequence* seenPlayerSequence;
			BehaviourSelector* rangeForAttackSelector;
			BehaviourSequence* inRangeToTargetSequence;

			//thd layer
			BehaviourSelector* moveSelector;
			BehaviourSequence* midRangeSequence;
			BehaviourSequence* farRangeSequence;
			BehaviourSequence* closeRangeSequence;

			//fourth layer
			BehaviourSequence* moveTimerSequence;
			BehaviourSelector* closeMoveSelector;
			BehaviourSequence* randomAttackSetSequence;

			// fifth layer
			//move timer is reused


			BehaviourState currentstate;
		};
	}
}