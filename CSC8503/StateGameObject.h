#pragma once
#include "GameObject.h"

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject : public GameObject  {
        public:
            StateGameObject(vector <Vector3 > testNodes);
            ~StateGameObject();

            virtual void Update(float dt);
			Vector3 getTargerPosition() {
				return targetPosition;
			};
			void setTargetPosition(Vector3 position) {
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
			Vector3 targetPosition;
            StateMachine* stateMachine;
            float counterX;
			float counterY;
			StateMachine* patrolMachine;
			StateMachine* escapeMachine;
			bool runAway;
			vector <Vector3 > nodes;
			int currentNode;
			bool reverse;
        };
    }
}
