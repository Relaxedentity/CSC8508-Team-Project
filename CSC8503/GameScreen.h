
#pragma once

#include "PushdownMachine.h"
#include "PushdownState.h"

#include "Window.h"
#include "GameEnd.h"
#include "PauseScreen.h"

#include "Gamelock.h"


using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;


namespace NCL {
	namespace CSC8503 {
		class GameScreen : public PushdownState
		{
		private:
			int state1;
			int state2;
			bool player1menuAwake;
			bool player2menuAwake;
		public:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override
			{
				if (GameLock::gamemod == 1) {
					//Debug::Print("Welcome To A Really Awesome Game!\n", Vector2(20, 12), Vector4(1, 1, 1, 1));
					//Debug::Print("Welcome To A Really Awesome Game!\n", Vector2(20, 12), Vector4(1, 1, 1, 1));
					Debug::Print("Press (F1)", Vector2(73, 7), Vector4(1, 1, 1, 1));
					Debug::Print("to open menu!", Vector2(73, 12), Vector4(1, 1, 1, 1));

					if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1) || player1menuAwake) {

						GameLock::Player1lock = true;
						GameLock::gamePause = true;
						player1menuAwake = false;
						*newState = new PauseScreen();
						return PushdownResult::Push;
					}
				}

				if (GameLock::gamemod == 2) {
					//Debug::Print("Welcome To A Really Awesome Game!\n", Vector2(20, 12), Vector4(1, 1, 1, 1));
					Debug::Print("Player1 Press", Vector2(73, 7), Vector4(1, 1, 1, 1));
					Debug::Print("(F1) to open", Vector2(73, 12), Vector4(1, 1, 1, 1));
					Debug::Print("menu!", Vector2(73, 17), Vector4(1, 1, 1, 1));
					Debug::Print("Player2 Press", Vector2(73, 22), Vector4(1, 1, 1, 1));
					Debug::Print("(F2) to open", Vector2(73, 27), Vector4(1, 1, 1, 1));
					Debug::Print("menu!", Vector2(73, 32), Vector4(1, 1, 1, 1));
					if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1)) {
						player1menuAwake = true;
						GameLock::Player1lock = true;
					}
					if (player1menuAwake) {
						Debug::Print("Use W and S", Vector2(5, 45), Vector4(1, 1, 1, 1));
						Debug::Print("select game mode", Vector2(5, 50), Vector4(1, 1, 1, 1));
						Debug::Print("Pause Menu", Vector2(5, 55), Vector4(1, 1, 1, 1));

						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::W)){
							state1 = state1 - 1 > 0 ? state1 - 1 : 2;
						}

						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::S)){
								state1 = state1 + 1 <= 2 ? state1 + 1 : 1;
						}
						switch (state1)
						{
						case 1:
							Debug::Print("Continue Game", Vector2(5, 70), Vector4(1, 0, 0, 1));
							Debug::Print("Exit Game to main menu", Vector2(5, 75));
							break;
						case 2:
							Debug::Print("Continue Game", Vector2(5, 70));
							Debug::Print("Exit Game to main menu", Vector2(5, 75), Vector4(1, 0, 0, 1));
							break;
						case 0:
							Debug::Print("Continue Game", Vector2(5, 70));
							Debug::Print("Exit Game to main menu", Vector2(5, 75));
							break;
						}

						if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
						{
							if (state1 == 1)
							{
								GameLock::Player1lock = false;
								player1menuAwake = false;
							}
							if (state1 == 2)
							{
								GameLock::gametime = GameLock::gamelength;
								GameLock::gamemod = 0;
								GameLock::gamestart = false;
								GameLock::Player1lock = true;
								GameLock::Mainmenuawake = true;
								return PushdownResult::ExitoMainMenu;
							}
						}
					}


					if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F2)) {
						player2menuAwake = true;
						GameLock::Player2lock = true;
					}
					if (player2menuAwake) {
						Debug::Print("Use UP and DOWN", Vector2(55, 45), Vector4(1, 1, 1, 1));
						Debug::Print("select game mode", Vector2(55, 50), Vector4(1, 1, 1, 1));
						Debug::Print("Pause Menu", Vector2(55, 55), Vector4(1, 1, 1, 1));

						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP)) {
							state2 = state2 - 1 > 0 ? state2 - 1 : 2;
						}

						if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN)) {
							state2 = state2 + 1 <= 2 ? state2 + 1 : 1;
						}

						switch (state2)
						{
						case 1:
							Debug::Print("Continue Game", Vector2(55, 70), Vector4(1, 0, 0, 1));
							Debug::Print("Player2 Exit", Vector2(55, 75));
							break;
						case 2:
							Debug::Print("Continue Game", Vector2(55, 70));
							Debug::Print("Player2 Exit", Vector2(55, 75), Vector4(1, 0, 0, 1));
							break;
						case 0:
							Debug::Print("Continue Game", Vector2(55, 70));
							Debug::Print("Player2 Exit", Vector2(55, 75));
							break;
						}

						if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
						{
							if (state2 == 1)
							{
								GameLock::Player2lock = false;
								player2menuAwake = false;
							}
							if (state2 == 2)
							{
								GameLock::gamemod = 1;
							}
						}
					}

				}

				if (GameLock::gametime <= 0) {
					GameLock::gamestart = false;
					GameLock::Player1lock = true;
					*newState = new GameEnd();
					return PushdownResult::Push;
				}
				return PushdownResult::NoChange;
			}
			void OnAwake() override
			{
				state1 = 0;
				state2 = 0;
				bool player1menuAwake = false;
				bool player2menuAwake = false;

				GameLock::gamePause = false;
			}
		protected:
			int coinsMined = 0;
		};
	}

}


