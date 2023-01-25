#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "HierarchicalState.h"
#include "PhysicsObject.h"
#include "Window.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject(vector <Vector3 > testNodes) {
	counterX = 0.0f;
	counterY = 0.0f;
	stateMachine = new StateMachine();
	patrolMachine = new StateMachine();
	escapeMachine = new StateMachine();
	runAway = false;
	nodes = testNodes;
	currentNode = 1;
	reverse = false;
	

	State* stateA = new State([&](float dt)-> void
		{
			this->Move(dt);
		}
	);
	State* stateB = new State([&](float dt)-> void
		{
			this->MoveBack(dt);
		}
	);
	State* stateC = new State([&](float dt)-> void
		{
			this->Move(dt);
		}
	);
	State* stateD = new State([&](float dt)-> void
		{
			this->Move(dt);
		}
	);
	State* stateE = new State([&](float dt)-> void
		{
			this->Escape(dt);
		}
	);
	HierarchicalState* Patrol = new HierarchicalState([&](float dt)-> void
		{
			this->patrolMachine;
		}
	);

	patrolMachine->AddState(stateA);
	//patrolMachine->AddState(stateB);
	//patrolMachine->AddState(stateC);
	//patrolMachine->AddState(stateD);
	stateMachine->AddState(Patrol);
	stateMachine->AddState(stateE);

	patrolMachine->AddTransition(new StateTransition(stateA, stateB,
		[&]()-> bool
		{	
			return this->currentNode == nodes.size() - 1;
		}
	));
	patrolMachine->AddTransition(new StateTransition(stateB, stateA,
		[&]()-> bool
		{
			return this->currentNode == 1;
		}
	));
	//patrolMachine->AddTransition(new StateTransition(stateB, stateC,
	//	[&]()-> bool
	//	{
	//		return this->counterX < 0.0f;
	//	}
	//));
	//patrolMachine->AddTransition(new StateTransition(stateC, stateA,
	//	[&]()-> bool
	//	{
	//		return this->counterY < 0.0f;
	//	}
	//));
	stateMachine->AddTransition(new StateTransition(Patrol, stateE,
		[&]()-> bool
		{
			Vector3 t = target->GetTransform().GetPosition() - GetTransform().GetPosition();
			return t.Length() < 15.0f;
		}
	));
	stateMachine->AddTransition(new StateTransition(stateE, Patrol,
		[&]()-> bool
		{
			Vector3 t = target->GetTransform().GetPosition() - GetTransform().GetPosition();
			return t.Length() > 20.0f;
		}
	));

}

StateGameObject::~StateGameObject() {
	delete stateMachine;
	delete escapeMachine;
	delete patrolMachine;
}

void StateGameObject::Update(float dt) {
	patrolMachine->Update(dt);
	stateMachine->Update(dt);
	escapeMachine->Update(dt);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::R)&& runAway == false) {
		runAway = true;
	}
	else if (Window::GetKeyboard()->KeyDown(KeyboardKeys::R) && runAway == true) {
		runAway = false;
	}

}

void StateGameObject::Move(float dt) {
	//Vector3 dir = target->GetTransform().GetPosition() - GetTransform().GetPosition;
	//Vector3 dir = Vector3(-80, 5, -80) - GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir*0.2f);
	Vector3 a = nodes[currentNode-1];
	Vector3 b = nodes[currentNode];
	Vector3 t = b - GetTransform().GetPosition();
	Vector3 dir = b - a;
	Vector3 dis = b - GetTransform().GetPosition();
	GetPhysicsObject()->AddForce(dis.Normalised() * 2.0f);
	counterX += dt;
	if ((b - GetTransform().GetPosition()).Length()<5.0f) {
		currentNode++;
	}
	int l = (GetTransform().GetPosition() - nodes[currentNode]).Length();
	if (l > 50.0f) {
		GetTransform().SetPosition(nodes[currentNode]);
	}
}
void StateGameObject::MoveBack(float dt) {
	//Vector3 dir = target->GetTransform().GetPosition() - GetTransform().GetPosition;
	//Vector3 dir = Vector3(-80, 5, -80) - GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir*0.2f);
	Vector3 b = nodes[currentNode - 1];
	Vector3 a = nodes[currentNode];
	Vector3 t = b - GetTransform().GetPosition();
	Vector3 dir = b - a;
	Vector3 dis = b - GetTransform().GetPosition();
	GetPhysicsObject()->AddForce(dis.Normalised() * 2.0f);
	
	counterX += dt;
	if ((b - GetTransform().GetPosition()).Length() < 5.0f) {
		currentNode--;
	}
	int l = (GetTransform().GetPosition() - nodes[currentNode]).Length();
	if (l > 50.0f) {
		GetTransform().SetPosition(nodes[currentNode]);
	}
}

void StateGameObject::MoveRight(float dt) {
	//Vector3 dir = Vector3(80, 5, 80) -GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir * 0.2f);
	//counterX -= dt;
	Vector3 a = nodes[1];
	Vector3 b = nodes[2];
	Vector3 dir = b - a;
	GetPhysicsObject()->AddForce(dir * 0.2f);
	counterX -= dt;
}
void StateGameObject::MoveUp(float dt) {
	//Vector3 dir = Vector3(80, 5, -80) -GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir * 0.2f);
	//counterY -= dt;
	Vector3 a = nodes[2];
	Vector3 b = nodes[3];
	Vector3 dir = b - a;
	GetPhysicsObject()->AddForce(dir * 0.2f);
	counterY -= dt;
}	

void StateGameObject::MoveDown(float dt) {
	//Vector3 dir = Vector3(-80, 5, 80) - GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir * 0.2f);
	//counterY += dt;

	Vector3 a = nodes[3];
	Vector3 b = nodes[4];
	Vector3 dir = b - a;
	GetPhysicsObject()->AddForce(dir * 0.2f);
	counterY += dt;
}

void StateGameObject::Escape(float dt) {
	Vector3 dir = GetTransform().GetPosition()-target->GetTransform().GetPosition();
	GetTransform().SetOrientation(target->GetTransform().GetOrientation());
	dir.y = 0;
	GetPhysicsObject()->AddForce(dir*2.0f);
}

