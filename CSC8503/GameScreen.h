
#pragma once
#include "PushdownMachine.h"
#include "PushdownState.h"

#include "Window.h"
#include "PauseScreen.h"
#include "GameEnd.h"

using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;


namespace NCL {
	namespace CSC8503 {
		class GameScreen : public PushdownState
		{
			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
					Debug::Print("Welcome To A Really Awesome Game!\n", Vector2(20, 5), Vector4(1.0, 0.0, 0.0, 0.0));
					Debug::Print("Press (P) To Pause Game And (F1) to main menu!\n", Vector2(15, 10), Vector4(1.0, 0.0, 0.0, 0.0));

					//DisplayPathfinding();
					//float dt = Window::GetTimer()->GetTimeDeltaSeconds();
					//float dt = w->GetTimer()->GetTimeDeltaSeconds();
					pauseReminder -= dt;
					//if (dt > 0.1f) {
					//	std::cout << "Skipping large time delta\n " << std::endl;
					//	continue; //must have hit a breakpoint or something to have a 1 second frame time!
					//}
					/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
						Window::ShowConsole(true);
					}
					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
						w->ShowConsole(false);
					}

					if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
						w->SetWindowPosition(0, 0);
					}*/

					if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P)) {
						*newState = new PauseScreen();
						Debug::Print("Press U To Continue", Vector2(40, 70), Vector4(1.0, 0.0, 0.0, 0.0));
						return PushdownResult::Push;
					}

					if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1)) {
						/*Debug::Print("Welcome to main menu! And player scored" + player->getScore(), Vector2(30, 60), Vector4(1.0, 0.0, 0.0, 0.0));
						return PushdownResult::Pop;
					}*/
						Debug::Print("Welcome to main menu! And player scored" + t->player->getScore(), Vector2(30, 60), Vector4(1.0, 0.0, 0.0, 0.0));
						Debug::Print("Start Game(Enter)", Vector2(40, 50), Vector4(1.0, 0.0, 0.0, 0.0));
						Debug::Print("Exit Game(Esc)", Vector2(40, 70), Vector4(1.0, 0.0, 0.0, 0.0));

						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
							*newState = new GameScreen();
							t->UpdateGame(dt);
							return PushdownResult::Push;

						}
						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
							return PushdownResult::Pop;
						}
						return PushdownResult::NoChange;
					}

					if (pauseReminder < 0) {
						Debug::Print("Time has elapsed. You have lost. Press (F1) back to main menu!\n", Vector2(40, 50), Vector4(1.0, 0.0, 0.0, 0.0));
						pauseReminder += 60.0f;
						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
						{
							return PushdownResult::Pop;
						}
					}
					if (t->GetGameWorld()->GetObjectCount() == 0 && t->getPlayer2() == NULL) {
						Debug::Print("You have won. Press (F1) back to main menu! And player scored: " + t->player->getScore(), Vector2(30, 60), Vector4(1.0, 0.0, 0.0, 0.0));
						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
						{
							return PushdownResult::Pop;
						}
					}
					if (t->GetGameWorld()->GetObjectCount() == 0 && t->getPlayer2() != NULL) {
						if (t->getPlayer()->getScore() > t->getPlayer2()->getScore()) {
							Debug::Print("Player 1 has won. Press (F1) back to main menu!\n", Vector2(40, 50), Vector4(1.0, 0.0, 0.0, 0.0));
							if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
							{
								return PushdownResult::Pop;
							}
						}
						else {
							Debug::Print("Player 2 has won. Press (F1) back to main menu!\n", Vector2(40, 50), Vector4(1.0, 0.0, 0.0, 0.0));
							if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
							{
								return PushdownResult::Pop;
							}
						}
					}
					if (rand() % 7 == 0) {
						coinsMined++;
					}

					t->UpdateGame(dt);
				/*}*/
				/*if (pauseReminder < 0) {
					std::cout << "Press P to pause game ,or F1 to return to main menu!\n";
					pauseReminder += 1.0f;
				}*/
				Window::DestroyGameWindow();

				return PushdownResult::NoChange;
			};


		protected:
			int coinsMined = 0;
			float pauseReminder = 240;
		};
	}

}


