#pragma once
#include "PushdownMachine.h"
#include "PushdownState.h"
#include "Window.h"
#include "TutorialGame.h"

#include "Gamelock.h"


using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;

namespace NCL {
	namespace CSC8503 {
		class GameLoad : public PushdownState
		{
		private:
			int state;
		public:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override {
	
				if (!GameLock::isloading1 && GameLock::loadingdone == 1) {
					GameLock::gamemod = 1;
					GameLock::gamestart = true;
					GameLock::Player1lock = false;
					*newState = new GameScreen();
					return PushdownResult::Push;
				}
				else if (!GameLock::isloading2 && GameLock::loadingdone == 2) {
					GameLock::gamemod = 2;
					GameLock::gamestart = true;
					GameLock::Player1lock = false;
					GameLock::Player2lock = false;
					*newState = new GameScreen();
					return PushdownResult::Push;
				}
				return PushdownState::NoChange;
			};
			void OnAwake() override
			{
				state = 0;
			}
		protected:
			int coinsMined = 0;
		};
	}
}

