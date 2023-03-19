#pragma once

using namespace reactphysics3d;
namespace NCL {
	namespace CSC8503 {
		class PushdownState
		{
		public:
			enum PushdownResult {
				Push, Pop, NoChange, Endgame, ExitoMainMenu, GotoMainMenu
			};
			PushdownState() {};
			virtual ~PushdownState() {}

			virtual PushdownResult OnUpdate(float dt, PushdownState** pushFunc) = 0;
			virtual void OnAwake() {}
			virtual void OnSleep() {}		
		protected:
		};
	}
}