#pragma once

#include "State.h";
#include "StateMachine.h";

namespace NCL {
	namespace CSC8503 {

		class HierarchicalState : public State {
		public:
			StateMachine* Hstatemachine;
			HierarchicalState() {
				Hstatemachine = new StateMachine;
			};
			HierarchicalState(StateUpdateFunction someFunc) {
				func = someFunc;
			}
			~HierarchicalState();

			void Update(float dt) {
				Hstatemachine->Update(dt);
			};
		};
	}
}
