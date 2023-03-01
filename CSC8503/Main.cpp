#include <reactphysics3d/reactphysics3d.h>
#include "Window.h"

#include "Debug.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "NetworkedGame.h"

#include "PushdownMachine.h"
#include "PushdownState.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>

class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override {
		std::cout << "Press U to unpause game!\n";
	}
};
TutorialGame* g;
Window* w;
GameObject* player;
GameObject* player2;

class GameScreen : public PushdownState {
	void OnAwake() override {
		w->GetTimer()->GetTimeDeltaSeconds();
	}
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		
		w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
		//TestPathfinding();
		//TestBehaviourTree();
		while (w->UpdateWindow()) {
			float dt = w->GetTimer()->GetTimeDeltaSeconds();
			pauseReminder -= dt;
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

			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::P)) {
				*newState = new PauseScreen();
				return PushdownResult::Push;
			}
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1)) {
				std::cout << "Returning to main menu!\n";
				return PushdownResult::Pop;
			}
			if (pauseReminder < 0) {
				std::cout << "Time has elapsed. You have lost. Returning to main menu!\n";
				pauseReminder += 60.0f;
				return PushdownResult::Pop;
			}
			if (rand() % 7 == 0) {
				coinsMined++;
			}

			w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

			g->UpdateGame(dt);
		}
		Window::DestroyGameWindow();

		return PushdownResult::NoChange;
	};

protected:
	int coinsMined = 0;
	float pauseReminder = 240;
};

class IntroScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN)) {
			*newState = new GameScreen();
			//g->InitWorld();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	};
	void OnAwake() override {
		//player->setScore(0);
		std::cout << "Welcome to a really awesome game!\n";
		std::cout << "Press Enter To Begin or escape to quit!\n";
		
	}
};
void TestPushdownAutomata(Window* w) {
	PushdownMachine machine(new IntroScreen());

	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (!machine.Update(dt)) {
			return;
		}
	}
}
class TestPacketReceiver : public PacketReceiver {
public:
	TestPacketReceiver(string name) {
		this->name = name;
	}

	void ReceivePacket(int type, GamePacket* payload, int source) {
		if (type == String_Message) {
			StringPacket* realPacket = (StringPacket*)payload;

			string msg = realPacket->GetStringFromData();

			std::cout << name << " received message: " << msg << std::endl;
		}
	}
protected:
	string name;
};

void TestNetworking() {
	NetworkBase::Initialise();

	TestPacketReceiver serverReceiver("Server");
	TestPacketReceiver clientReceiver("Client");

	int port = NetworkBase::GetDefaultPort();

	GameServer* server = new GameServer(port, 1);
	GameClient* client = new GameClient();

	server->RegisterPacketHandler(String_Message, &serverReceiver);
	client->RegisterPacketHandler(String_Message, &clientReceiver);

	bool canConnect = client->Connect(127, 0, 0, 1, port);

	for (int i = 0; i < 100; ++i) {
		StringPacket s = StringPacket("Server says hello! " + std::to_string(i));
		server->SendGlobalPacket(s);
		StringPacket s1 = StringPacket("Client says hello! " + std::to_string(i));
		client->SendPacket(s1);

		server->UpdateServer();
		client->UpdateClient();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	NetworkBase::Destroy();
}

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
	w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);
	g = new NetworkedGame();
	player = g->getPlayer();
	if (g->getPlayer2() != NULL) {
		player2 = g->getPlayer2();
	}
	TestPushdownAutomata(w);
	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(false);

	
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
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