#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "HierarchicalState.h"
#include <reactphysics3d/reactphysics3d.h>
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
			Vector3 t = Vector3(getTarget()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition());
			return t.Length() < 15.0f;
		}
	));
	stateMachine->AddTransition(new StateTransition(stateE, Patrol,
		[&]()-> bool
		{
			Vector3 t = Vector3(getTarget()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition());
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
	Vector3 t = b - GetPhysicsObject()->getTransform().getPosition();
	Vector3 dir = b - a;
	Vector3 dis = b - GetPhysicsObject()->getTransform().getPosition();
	Vector3 force = dis.Normalised() * 2.0f;
	GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
	counterX += dt;
	if ((b - GetPhysicsObject()->getTransform().getPosition()).Length()<5.0f) {
		currentNode++;
	}
	int l = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - nodes[currentNode]).Length();
	if (l > 50.0f) {
		Vector3 position = nodes[currentNode];
		GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(position.x, position.y, position.z), GetPhysicsObject()->getTransform().getOrientation()));
	}
}
void StateGameObject::MoveBack(float dt) {
	//Vector3 dir = target->GetTransform().GetPosition() - GetTransform().GetPosition;
	//Vector3 dir = Vector3(-80, 5, -80) - GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir*0.2f);
	Vector3 b = nodes[currentNode - 1];
	Vector3 a = nodes[currentNode];
	Vector3 t = b - GetPhysicsObject()->getTransform().getPosition();
	Vector3 dir = b - a;
	Vector3 dis = b - GetPhysicsObject()->getTransform().getPosition();
	Vector3 force = dis.Normalised() * 2.0f;
	GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
	
	counterX += dt;
	if ((b - GetPhysicsObject()->getTransform().getPosition()).Length() < 5.0f) {
		currentNode--;
	}
	int l = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - nodes[currentNode]).Length();
	if (l > 50.0f) {
		Vector3 position = nodes[currentNode];
		GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(position.x, position.y, position.z), GetPhysicsObject()->getTransform().getOrientation()));
	}
}

void StateGameObject::MoveRight(float dt) {
	//Vector3 dir = Vector3(80, 5, 80) -GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir * 0.2f);
	//counterX -= dt;
	Vector3 a = nodes[1];
	Vector3 b = nodes[2];
	Vector3 dir = b - a;
	Vector3 force = dir * 2.0f;
	GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
	counterX -= dt;
}
void StateGameObject::MoveUp(float dt) {
	//Vector3 dir = Vector3(80, 5, -80) -GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir * 0.2f);
	//counterY -= dt;
	Vector3 a = nodes[2];
	Vector3 b = nodes[3];
	Vector3 dir = b - a;
	Vector3 force = dir * 2.0f;
	GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
	counterY -= dt;
}	

void StateGameObject::MoveDown(float dt) {
	//Vector3 dir = Vector3(-80, 5, 80) - GetTransform().GetPosition();
	//GetPhysicsObject()->AddForce(dir * 0.2f);
	//counterY += dt;

	Vector3 a = nodes[3];
	Vector3 b = nodes[4];
	Vector3 dir = b - a;
	Vector3 force = dir * 2.0f;
	GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
	counterY += dt;
}

void StateGameObject::Escape(float dt) {
	Vector3 dir = GetPhysicsObject()->getTransform().getPosition() - target->GetPhysicsObject()->getTransform().getPosition();
	GetPhysicsObject()->setTransform(reactphysics3d::Transform(GetPhysicsObject()->getTransform().getPosition(), target->GetPhysicsObject()->getTransform().getOrientation()));
	dir.y = 0;
	Vector3 force = dir * 2.0f;
	GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
}

