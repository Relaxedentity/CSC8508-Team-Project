#pragma once
#include "PushdownMachine.h"
#include "PushdownState.h"
#include "Window.h"
#include "TutorialGame.h"

#include "Gamelock.h"


using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;

TutorialGame* t;
namespace NCL {
	namespace CSC8503 {
		class GameEnd : public PushdownState
		{
		private:
			int state;
		public:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				GameLock::gametime = 0;
				Debug::Print("Game End.\n", Vector2(40, 50), Vector4(1, 1, 1, 1));
				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
				{
					state = state - 1 > 0 ? state - 1 : 1;
				}

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
				{
					state = state + 1 <= 1 ? state + 1 : 1;
				}

				switch (state)
				{
				case 1:
					Debug::Print("Exit to main menu", Vector2(40, 55), Vector4(1, 0, 0, 1));
					break;
				case 0:
					Debug::Print("Exit to main menu", Vector2(40, 55));
					break;
					/*if (t->GetGameWorld()->GetObjectCount() == 0 && t->getPlayer2() == NULL) {
						Debug::Print("You have won. And player scored: " + t->player->getScore(), Vector2(30, 60), Vector4(1.0, 0.0, 0.0, 0.0));
						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
						{
							return PushdownResult::Pop;
						}
					}
					if (t->GetGameWorld()->GetObjectCount() == 0 && t->getPlayer2() != NULL) {
						//if (t->getPlayer()->getScore() > t->getPlayer2()->getScore()) {
						if (false) {
							Debug::Print("Player 1 has won.\n", Vector2(40, 50), Vector4(1.0, 0.0, 0.0, 0.0));
							if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
							{
								return PushdownResult::Pop;
							}
						}
						else {
							Debug::Print("Player 2 has won.\n", Vector2(40, 50), Vector4(1.0, 0.0, 0.0, 0.0));
							if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
							{
								return PushdownResult::Pop;
							}
						}
					}*/
				}
				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RIGHT))
				{
					if (state == 1) {
						GameLock::gametime = 300;
						GameLock::gamemod = 0;
						GameLock::gamestart = false;
						GameLock::Player1lock = true;
						return PushdownState::ExitoMainMenu;
					}
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

