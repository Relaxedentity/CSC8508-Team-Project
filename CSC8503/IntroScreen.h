#pragma once
#include "PushdownMachine.h"
#include "PushdownState.h"
#include "GameScreen.h"
#include "Window.h"
#include "GameIntroduction.h"
#include "PauseScreen.h"
#include "GameEnd.h"
#include"TutorialGame.h"

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

				Debug::Print("Use Up and Down to select game mode", Vector2(10, 25), Vector4(1, 1, 1, 1));
				Debug::Print("Game Menu", Vector2(10, 45), Vector4(1, 1, 1, 1));

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP))
				{
					state = state - 1 >0?state-1:3;
				}

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN))
				{
					state = state +1 <= 3 ? state + 1 : 1;
				}

				switch (state)
				{
				case 1:
					Debug::Print("Start Single Game", Vector2(10, 50),Vector4(1,0,0,1));
					Debug::Print("Start Multiple Game", Vector2(10, 55));
					Debug::Print("Game Introduction", Vector2(10, 60));
					break;
				case 2:
					Debug::Print("Start Single Game", Vector2(10, 50));
					Debug::Print("Start Multiple Game", Vector2(10, 55), Vector4(1, 0, 0, 1));
					Debug::Print("Game Introduction", Vector2(10, 60));
					break;
				case 3:
					Debug::Print("Start Single Game", Vector2(10, 50));
					Debug::Print("Start Multiple Game", Vector2(10, 55));
					Debug::Print("Game Introduction", Vector2(10, 60),Vector4(1, 0, 0, 1));
					break;
				case 0:
					Debug::Print("Start Single Game", Vector2(10, 50));
					Debug::Print("Start Mutiple Game", Vector2(10, 55));
					Debug::Print("Game Introduction", Vector2(10, 60));
					break;
				}

 				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
				{
					if (state == 1 || state == 2)
					{
						*newState = new GameScreen();
						t->InitWorld();
						//g->InitWorld();
						return PushdownResult::Push;

						//Game Start
						//TutorialGame::game_Start = true;
						//*newState = new GameScreen();
						//return PushdownResult::Push;
					}
					if (state == 3)
					{
						*newState = new GameIntroduction();
						return PushdownResult::Push;
					}
				}

				return PushdownResult::NoChange;
			};

			void OnAwake() override 
			{
				state = 0;
			}
		};
	}
}