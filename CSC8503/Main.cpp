#include <reactphysics3d/reactphysics3d.h>
#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

#include "PushdownMachine.h"
#include "IntroScreen.h"
#include "GameScreen.h"
#include "GameIntroduction.h"
#include "GameEnd.h"
#include "PauseScreen.h"
#include "PushdownState.h"

#include "Gamelock.h"
#include "Sound.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/

int main() {
	GameLock::Player1lock = true;//menu value init
	GameLock::gamemod = 0;///////////////////////////
	GameLock::gamestart = false;////////////////
	GameLock::gametime = 300;////////////////////////////
	bool NotConfirmExit = true;///////////////////////////
	PushdownMachine* machine = new PushdownMachine(new IntroScreen());//menuinit


	Window* w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);
	NetworkedGame* g = new NetworkedGame();
	//g = new TutorialGame();
	PlayerObject* player = g->getPlayer();
	if (g->getPlayer2() != NULL) {
		PlayerObject* player2 = g->getPlayer2();
	}
	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(false);

	
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (!NotConfirmExit) {//exit game
			Window::DestroyGameWindow();
			break;
		}
		NotConfirmExit = machine->Update(dt);
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}