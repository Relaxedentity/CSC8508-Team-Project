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
		class GameEnd : public PushdownState
		{
		private:
			int state;
		public:
			PushdownResult OnUpdate(float dt, PushdownState** newState) override {
				GameLock::gametime = 0;
				Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
				Vector2 screenSize = Window::GetWindow()->GetScreenSize();
				if (GameLock::gamemod == 1) {
					GameLock::SingleEndMenuawake = true;
					if (GameLock::EndButtonPos.size() > 0) {
						for (int i = 1; i <= GameLock::EndButtonPos.size(); i++) {
							vector<Vector2> MappingPos = GameLock::EndButtonPos[i - 1];
							if (buttonmapping(MappingPos[0], MappingPos[1], screenMouse, screenSize)) {
								state = i;
								if (state == 1) {
									GameLock::SingleExitBtnChange = true;
									break;
								}
							}
							else {
								state = 0;
								GameLock::SingleExitBtnChange = false;
							}
						}
					}
				}
				if (GameLock::gamemod == 2) {
					GameLock::CoopEndMenuawake = true;
					if (GameLock::CoopButtonPos.size() > 0) {
						for (int i = 1; i <= GameLock::CoopButtonPos.size(); i++) {
							vector<Vector2> MappingPos = GameLock::CoopButtonPos[i - 1];
							if (buttonmapping(MappingPos[0], MappingPos[1], screenMouse, screenSize)) {
								state = i;
								if (state == 1) {
									GameLock::CoopExitBtnChange = true;
									break;
								}
							}
							else {
								state = 0;
								GameLock::CoopExitBtnChange = false;
							}
						}
					}
				}


				if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT))
				{
					if (state == 1) {
						GameLock::istoString = false;
						GameLock::isto2String = false;
						GameLock::gametime = 10;
						GameLock::gamemod = 0;
						GameLock::gamestart = false;
						GameLock::Player1lock = true;
						GameLock::Mainmenuawake = true;
						GameLock::SingleEndMenuawake = false;
						GameLock::CoopEndMenuawake = false;
						return PushdownState::GotoMainMenu;
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

