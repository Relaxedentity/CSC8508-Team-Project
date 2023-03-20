#pragma once
#include "PushdownMachine.h"
#include "PushdownState.h"
#include "Window.h"

#include "Gamelock.h"


using namespace NCL;
using namespace CSC8503;
using namespace NCL::CSC8503;
namespace NCL {
	namespace CSC8503 {
		class GameIntroduction : public PushdownState
		{
		private:
			int state;
		public:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
				Vector2 screenSize = Window::GetWindow()->GetScreenSize();
					if (GameLock::BackButtonPos.size() > 0) {
						for (int i = 1; i <= GameLock::BackButtonPos.size(); i++) {
							vector<Vector2> MappingPos = GameLock::BackButtonPos[i - 1];
							if (buttonmapping(MappingPos[0], MappingPos[1], screenMouse, screenSize)) {
								state = i;
								if (state == 1) {
									GameLock::introBtnChange = true;
									break;
								}
							}
							else {
								state = 0;
								GameLock::introBtnChange = false;
							}
						}
					}


				if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT))
				{
					if (state == 1) {
						GameLock::Mainmenuawake = true;
						GameLock::IntroMenuawake = false;
						return PushdownState::Pop;
					}
				}
				return PushdownState::NoChange;
			};
			void OnAwake() override
			{
				state = 0;
			}
			bool buttonmapping(Vector2 leftT, Vector2 rightB, Vector2 screenMouse, Vector2 screenSize) {
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
		protected:
			int coinsMined = 0;
		};
	}
}
