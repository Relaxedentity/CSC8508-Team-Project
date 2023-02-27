#pragma once
#include "PushdownMachine.h"
#include "PushdownState.h"
#include "Window.h"

using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;

namespace NCL {
	namespace CSC8503 {
		class GameIntroduction : public PushdownState
		{
		private:
		public:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override {

				Debug::Print("1.Use WASD to controll the goat.", Vector2(0, 25), Vector4(1, 1, 1, 1));
				Debug::Print("2.*****************************,", Vector2(0, 30), Vector4(1, 1, 1, 1));
				Debug::Print("********************************", Vector2(0, 35), Vector4(1, 1, 1, 1));
				Debug::Print("3.******************************", Vector2(0, 40), Vector4(1, 1, 1, 1));
				Debug::Print("Back", Vector2(0, 55), Vector4(1, 0, 0, 1));

				if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
				{
					return PushdownResult::Pop;
				}

				return PushdownResult::NoChange;
			};

			void OnAwake() override
			{

			}
		};
	}
}