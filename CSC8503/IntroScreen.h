#pragma once
#include "Gamelock.h"

#include "PushdownMachine.h"
#include "PushdownState.h"
#include "GameScreen.h"
#include "Window.h"
#include "GameIntroduction.h"
#include "PauseScreen.h"
#include "GameEnd.h"

using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;

namespace NCL {
	namespace CSC8503 {
		class IntroScreen : public PushdownState
		{
		private:
			int state;
		public:

			PushdownResult OnUpdate(float dt,PushdownState** newState) override {
				
				Debug::Print("Use Up and Down to select game mode", Vector2(20, 22), Debug::BLACK);
				Debug::Print("Game Menu", Vector2(60, 40), Debug::BLACK);

				GameLock::Player1lock = true;
				 
				Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
				Vector2 screenSize = Window::GetWindow()->GetScreenSize();


				/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
				{
					state = state - 1 >0?state-1:4;
				}

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
				{
					state = state +1 <= 4 ? state + 1 : 1;
				}*/

				for (int i = 1; i <= GameLock::buttonPos.size(); i++) {
					vector<Vector2> MappingPos = GameLock::buttonPos[i];
					if (buttonmapping(MappingPos[0], MappingPos[1], screenMouse, screenSize)) {
						state = i;
						break;
					}
					else {
						state = 0;
					}
				}

				/*switch (state)
				{
				case 1:
					Debug::Print("Start Single Game", Vector2(60, 50),Vector4(1,0,0,1));
					Debug::Print("Start Multiple Game", Vector2(60, 55), Debug::BLACK);
					Debug::Print("Game Introduction", Vector2(60, 60), Debug::BLACK);
					Debug::Print("Exit Game", Vector2(60, 65), Debug::BLACK);
					break;
				case 2:
					Debug::Print("Start Single Game", Vector2(60, 50), Debug::BLACK);
					Debug::Print("Start Multiple Game", Vector2(60, 55), Vector4(1, 0, 0, 1));
					Debug::Print("Game Introduction", Vector2(60, 60), Debug::BLACK);
					Debug::Print("Exit Game", Vector2(60, 65),Debug::BLACK);
					break;
				case 3:
					Debug::Print("Start Single Game", Vector2(60, 50), Debug::BLACK);
					Debug::Print("Start Multiple Game", Vector2(60, 55), Debug::BLACK);
					Debug::Print("Game Introduction", Vector2(60, 60),Vector4(1, 0, 0, 1));
					Debug::Print("Exit Game", Vector2(60, 65), Debug::BLACK);
					break;
				case 4:
					Debug::Print("Start Single Game", Vector2(60, 50), Debug::BLACK);
					Debug::Print("Start Multiple Game", Vector2(60, 55), Debug::BLACK);
					Debug::Print("Game Introduction", Vector2(60, 60), Debug::BLACK);
					Debug::Print("Exit Game", Vector2(60, 65), Vector4(1, 0, 0, 1));
					break;
				case 0:
					Debug::Print("Start Single Game", Vector2(60, 50), Debug::BLACK);
					Debug::Print("Start Multiple Game", Vector2(60, 55), Debug::BLACK);
					Debug::Print("Game Introduction", Vector2(60, 60), Debug::BLACK);
					Debug::Print("Exit Game", Vector2(60, 65), Debug::BLACK);
					break;
				}*/


 				if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT))
				{
					if (state == 1)
					{
						//iflock = false;
						GameLock::gamemod = 1;
						GameLock::gamestart = true;
						GameLock::Player1lock = false;
						*newState = new GameScreen();
						//t->InitWorld();
						//g->InitWorld();
						return PushdownResult::Push;

						//Game Start
						//TutorialGame::game_Start = true;
						//*newState = new GameScreen();
						//return PushdownResult::Push;
					}
					if (state == 2) {
						GameLock::gamemod = 2;
						GameLock::gamestart = true;
						GameLock::Player1lock = false;
						GameLock::Player2lock = false;
						*newState = new GameScreen();
						return PushdownResult::Push;
					}
					if (state == 3)
					{
						*newState = new GameIntroduction();
						return PushdownResult::Push;
					}
					if (state == 4)
					{
						return PushdownResult::Endgame;
					}
				}

				return PushdownResult::NoChange;
			};

			void OnAwake() override 
			{
				state = 0;
			}
			bool buttonmapping(Vector2 leftT, Vector2 rightB, Vector2 screenMouse,Vector2 screenSize) {
				float yMaxMapping = (1 - leftT.y) / 2;
				float yMinMapping = (1 - rightB.y) / 2;
				float xMaxMapping = (1 - rightB.x) / 2;
				float xMinMapping = (1 - leftT.x) / 2;
				if (screenMouse.y <= yMaxMapping * screenSize.y
					&& screenMouse.y >= yMinMapping * screenSize.y
					&& screenMouse.x <= xMaxMapping * screenSize.x
					&& screenMouse.x >= xMinMapping * screenSize.x)
					return true;
				else
					return false;
				
			}
		};
	}
}