#pragma once
#include "GameObject.h"
#include "BehaviourSequence.h"

namespace NCL {
	namespace CSC8503 {
		class StateMachine;
		class BTreeObject : public GameObject {
		public:
			BTreeObject(std::vector<NCL::Maths::Vector3> testNodes);
			~BTreeObject();

			virtual void Update(float dt);
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
			BehaviourSequence* sequence;
			BehaviourState currentstate;
		};
	}
}
