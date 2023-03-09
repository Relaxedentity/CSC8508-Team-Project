#pragma once
#include "GameObject.h"

#include "BehaviourSequence.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourAction.h"

namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class BTreeObject : public GameObject {
		public:
			BTreeObject(GameWorld* world,std::vector<NCL::Maths::Vector3> testNodes);
			~BTreeObject();

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
		protected:
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
			BehaviourSequence* InRangeToTargetSequence;
			
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
