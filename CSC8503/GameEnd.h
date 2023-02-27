#pragma once
#include "PushdownMachine.h"
#include "PushdownState.h"
#include "Window.h"
#include "TutorialGame.h"


using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;

TutorialGame* t;
namespace NCL {
	namespace CSC8503 {
		class GameEnd : public PushdownState
		{
			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				if (pauseReminder < 0) {
					Debug::Print("Time has elapsed. You have lost.\n", Vector2(40, 50), Vector4(1.0, 0.0, 0.0, 0.0));
					pauseReminder += 60.0f;
					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
					{
						return PushdownResult::Pop;
					}
				}
				if (t->GetGameWorld()->GetObjectCount() == 0 && t->getPlayer2() == NULL) {
					Debug::Print("You have won. And player scored: " + t->player->getScore(), Vector2(30, 60), Vector4(1.0, 0.0, 0.0, 0.0));
					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
					{
						return PushdownResult::Pop;
					}
				}
				if (t->GetGameWorld()->GetObjectCount() == 0 && t->getPlayer2() != NULL) {
					if (t->getPlayer()->getScore() > t->getPlayer2()->getScore()) {
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
				}
				
			/*				
				Debug::Print("Your Score is: " + player->getScore(), Vector2(30, 50), Vector4(1, 1, 1, 1));
				if (TutorialGame::score <= 30)
				{
					Debug::Print("Sorry,You lose!" + player->getScore(), Vector2(30, 45), Vector4(1, 0, 0, 1));
				}
				else
				{
					Debug::Print("You Win!" + player->getScore(), Vector2(35, 45), Vector4(0, 1, 0, 1));
				}
				return PushdownResult::NoChange;*/
			}
			void OnAwake() override
			{

			}
		protected:
			int coinsMined = 0;
			float pauseReminder = 240;
		};
	}
}

