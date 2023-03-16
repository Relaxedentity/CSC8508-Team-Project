#pragma once
#include "GameObject.h"

#include "BehaviourSequence.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourAction.h"

namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class BossAI : public GameObject {
		public:
			BossAI(GameWorld* world, std::vector<NCL::Maths::Vector3> mapNodes);
			~BossAI();

			void CreateBehaviourTree();

			void UpdateBoss(float dt, NCL::Maths::Vector3& playePos) ;



			//void CreatePath(Vector3& position);
			//void DisplayPath();
			//void WalkPath(Vector3& destination);

			
		protected:

			void CreatePath(NCL::Maths::Vector3& position);
			void DisplayPath();
			void WalkPath(NCL::Maths::Vector3& destination);
			
			NCL::Maths::Vector3 targetPosition;

			std::vector<NCL::Maths::Vector3> pathNodes;
			int currentNode;

			NCL::Maths::Vector3 dest;
			NCL::Maths::Vector3 currPlayerPos;


			bool destNotArrived;
			bool seenPlayer;
			bool foundPath;
			bool walkToPlayer;

			int nodeIndex;
			int rNum;

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