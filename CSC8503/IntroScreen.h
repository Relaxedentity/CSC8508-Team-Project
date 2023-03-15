#pragma once
#include "Gamelock.h"

#include "PushdownMachine.h"
#include "PushdownState.h"
#include "GameScreen.h"
#include "Window.h"
#include "GameIntroduction.h"
#include "PauseScreen.h"
#include "GameEnd.h"
#include "GameLoad.h"

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
				
		/*		Debug::Print("Use Up and Down to select game mode", Vector2(20, 22), Debug::BLACK);
				Debug::Print("Game Menu", Vector2(60, 40), Debug::BLACK);*/

				GameLock::Player1lock = true;
				 
				Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
				Vector2 screenSize = Window::GetWindow()->GetScreenSize();

				if (GameLock::IntroButtonPos.size() > 0) {
					for (int i = 1; i <= GameLock::IntroButtonPos.size(); i++) {
						vector<Vector2> MappingPos = GameLock::IntroButtonPos[i-1];
						if (buttonmapping(MappingPos[0], MappingPos[1], screenMouse, screenSize)) {
							state = i;
							if (state == 1) {
								GameLock::normalBtnChange = true;
								break;
							} 
							else if (state == 2) {
								GameLock::coopBtnChange = true;
								break;
							}
							else if (state == 3) {
								GameLock::introBtnChange = true;
								break;
							}
							else if (state == 4) {
								GameLock::exitBtnChange = true;
							}
						
						}
						else {
							state = 0;
							GameLock::normalBtnChange = false;
							GameLock::coopBtnChange = false;
							GameLock::introBtnChange = false;
							GameLock::exitBtnChange = false;
						}
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


 				if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT))
				{
					if (state == 1)
					{
						//iflock = false;
						GameLock::isloading1 = true;
						GameLock::Mainmenuawake = false;
						*newState = new GameLoad();
						return PushdownResult::Push;

						//Game Start
						//TutorialGame::game_Start = true;
						//*newState = new GameScreen();
						//return PushdownResult::Push;
					}
					if (state == 2) {
						GameLock::isloading2 = true;
						GameLock::Mainmenuawake = false;
						*newState = new GameLoad();
						return PushdownResult::Push;
					}
					if (state == 3)
					{
						GameLock::IntroMenuawake = true;
						GameLock::Mainmenuawake = false;
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
				float yMinMapping = (1 - leftT.y) / 2;
				float yMaxMapping = (1 - rightB.y) / 2;
				float xMaxMapping = (1 + rightB.x) / 2;
				float xMinMapping = (1 + leftT.x) / 2;
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