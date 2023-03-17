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
		class PauseScreen : public PushdownState
		{
		private:
			int state;
		public:
			PushdownResult OnUpdate(float dt, PushdownState** pushFunc) override
			{
				

				Debug::Print("Use W and S to select game mode", Vector2(5, 25), Vector4(1, 1, 1, 1));
				Debug::Print("Pause Menu", Vector2(5, 45), Vector4(1, 1, 1, 1));

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::W))
				{
					state = state - 1 > 0 ? state - 1 : 2;
				}

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::S))
				{
					state = state + 1 <= 2 ? state + 1 : 1;
				}

				switch (state)
				{
				case 1:
					Debug::Print("Continue Game", Vector2(5, 50), Vector4(1, 0, 0, 1));
					Debug::Print("Exit Game to main menu", Vector2(5, 55));
					break;
				case 2:
					Debug::Print("Continue Game", Vector2(5, 50));
					Debug::Print("Exit Game to main menu", Vector2(5, 55), Vector4(1, 0, 0, 1));
					break;
				case 0:
					Debug::Print("Continue Game", Vector2(5, 50));
					Debug::Print("Exit Game to main menu", Vector2(5, 55));
					break;
				}

				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
				{
					if (state == 1)
					{
						GameLock::Player1lock = false;
						return PushdownResult::Pop;
					}
					if (state == 2)
					{
						GameLock::gametime = GameLock::gamelength;
						GameLock::gamemod = 0;
						GameLock::gamestart = false;
						GameLock::Player1lock = true;
						return PushdownResult::ExitoMainMenu;
					}
				}


				if (GameLock::gametime <= 0) {

					return PushdownResult::Pop;
				}

				return PushdownResult::NoChange;
			}

			void OnAwake() override
			{
				state = 0;
			}
		};

	}
}

#pragma once
