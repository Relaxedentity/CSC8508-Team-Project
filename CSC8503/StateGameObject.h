#pragma once
#include "GameObject.h"

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject : public GameObject  {
        public:
            StateGameObject(GameWorld* world, std::vector<NCL::Maths::Vector3> testNodes);
            ~StateGameObject();

            void Update(float dt) override;
			NCL::Maths::Vector3 getTargerPosition() {
				return targetPosition;
			};
			void setTargetPosition(NCL::Maths::Vector3 position) {
				targetPosition = position;
			};
			GameObject* getTarget() {
				return target;
			};
			void setTarget(GameObject* gametarget) {
				target = gametarget;
			};
        protected:
            void Move(float dt);
			void MoveBack(float dt);
            void MoveRight(float dt);
			void MoveUp(float dt);
			void MoveDown(float dt);
			void Escape(float dt);
			GameObject* target;
			NCL::Maths::Vector3 targetPosition;
            StateMachine* stateMachine;
            float counterX;
			float counterY;
			StateMachine* patrolMachine;
			StateMachine* escapeMachine;
			bool runAway;
			std::vector<NCL::Maths::Vector3> nodes;
			int currentNode;
			bool reverse;
        };
    }
}
