#pragma once
#include "PushdownMachine.h"
#include "PushdownState.h"

#include "Window.h"

using namespace NCL;
using namespace CSC8503;

using namespace NCL::CSC8503;
namespace NCL {
	namespace CSC8503 {
		class PauseScreen : public PushdownState
		{
			PushdownResult OnUpdate(float dt, PushdownState** pushFunc) override
			{
				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U))
				{
					return PushdownResult::Pop;
				}

				return PushdownResult::NoChange;
			}

			void OnAwake() override
			{
				std::cout << "Press U to unpause game" << std::endl;
			}
		};

	}
}

#pragma once
