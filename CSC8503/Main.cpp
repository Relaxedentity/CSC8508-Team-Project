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


#include "PushdownState.h"
#include "GameObject.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

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
/*vector <Vector3> testNodes;
void TestPathfinding() {
	NavigationGrid grid("TestGrid2.txt");
	NavigationPath outPath;

	Vector3 startPos(100, 0, 0);
	Vector3 endPos(80, 0, 100);

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void DisplayPathfinding() {
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}*/

void TestStateMachine() {
	StateMachine* testMachine = new StateMachine();
	int data = 0;

	State* A = new State([&](float dt)->void
		{
			std::cout << "I’m in state A!\n";
			data++;
		}
	);
	State* B = new State([&](float dt)->void
		{
			std::cout << "I’m in state B!\n";
			data--;
		}
	);
	StateTransition* stateAB = new StateTransition(A, B, [&](void)->bool
		{
			return data > 10;
		}
	);
	StateTransition* stateBA = new StateTransition(B, A, [&](void)->bool
		{
			return data < 0;
		}
	);
	testMachine->AddState(A);
	testMachine->AddState(B);
	testMachine->AddTransition(stateAB);
	testMachine->AddTransition(stateBA);

	for (int i = 0; i < 100; ++i) {
		testMachine->Update(1.0f);
	}
}

void TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;
	BehaviourAction* findKey = new BehaviourAction("Find Key",
		[&](float dt, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				std::cout << "Looking for a key!\n";
				behaviourTimer = rand() % 100;
				state = Ongoing;
			}
			else if (state == Ongoing) {
				behaviourTimer -= dt;
				if (behaviourTimer <= 0.0f) {
					std::cout << "Found a key!\n";
					return Success;
				}
			}
			return state; //will be ’ongoing ?until success
		}
	);
	BehaviourAction* goToRoom = new BehaviourAction("Go To Room",
		[&](float dt, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				std::cout << "Going to the loot room!\n";
				state = Ongoing;
			}
			else if (state == Ongoing) {
				distanceToTarget -= dt;
				if (distanceToTarget <= 0.0f) {
					std::cout << "Reached room!\n";
					return Success;
				}
			}
			return state; //will be ’ongoing ?until success
		}
	);
	BehaviourAction* openDoor = new BehaviourAction("Open Door",
		[&](float dt, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				std::cout << "Opening Door!\n";
				return Success;
			}
			return state;
		}
	);
	BehaviourAction* lookForTreasure = new BehaviourAction(
		"Look For Treasure",
		[&](float dt, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				std::cout << "Looking for treasure !\n";
				return Ongoing;
			}
			else if (state == Ongoing) {
				bool found = rand() % 2;
				if (found) {
					std::cout << "I found some treasure !\n";
					return Success;
				}
				std::cout << "No treasure in here ...\n";
				return Failure;
			}
			return state;
		}
	);
	BehaviourAction* lookForItems = new BehaviourAction(
		"Look For Items",
		[&](float dt, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				std::cout << "Looking for items!\n";
				return Ongoing;
			}
			else if (state == Ongoing) {
				bool found = rand() % 2;
				if (found) {
					std::cout << "I found some items!\n";
					return Success;
				}
				std::cout << "No items in here ...\n";
				return Failure;
			}
			return state;
		}
	);
	BehaviourSequence* sequence =
		new BehaviourSequence("Room Sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);

	BehaviourSelector* selection =
		new BehaviourSelector("Loot Selection");
	selection->AddChild(lookForTreasure);
	selection->AddChild(lookForItems);

	BehaviourSequence* rootSequence =
		new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);

	for (int i = 0; i < 5; ++i) {
		rootSequence->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 250;
		BehaviourState state = Ongoing;
		std::cout << "We’re going on an adventure !\n";
		while (state == Ongoing) {
			state = rootSequence->Execute(1.0f); //fake dt
		}
		if (state == Success) {
			std::cout << "What a successful adventure !\n";
		}
		else if (state == Failure) {
			std::cout << "What a waste of time!\n";
		}
	}
	std::cout << "All done!\n";


}

TutorialGame* g;
Window* w;
GameObject* player;
GameObject* player2;

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
	GameLock::Player1lock = true;//menu value init
	GameLock::gamemod = 0;///////////////////////////
	GameLock::gamestart = false;////////////////
	GameLock::gametime = 15;////////////////////////////
	bool NotConfirmExit = true;///////////////////////////
	PushdownMachine* machine = new PushdownMachine(new IntroScreen());//menuinit


	w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);
	g = new NetworkedGame();
	//g = new TutorialGame();
	player = g->getPlayer();
	if (g->getPlayer2() != NULL) {
		player2 = g->getPlayer2();
	}
	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(false);

	
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	//TestPathfinding();
	//TestBehaviourTree();
	while (w->UpdateWindow()) {
		//DisplayPathfinding();
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