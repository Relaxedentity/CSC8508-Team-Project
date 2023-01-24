#include "BTreeObject.h"
#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "../Build/CSC8503CoreClasses/HierarchicalState.h"
#include "PhysicsObject.h"
#include "Window.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include <Debug.cpp>


using namespace NCL;
using namespace CSC8503;

BTreeObject::BTreeObject(vector <Vector3 > testNodes) {
	nodes = testNodes;
	currentNode = 1;
	float t;
	float restTime;
	GameObject* a;
	float l1;
	float l2;
	BehaviourAction* Patrol = new BehaviourAction("Patrolling\n",
		[&](float dt, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				currentstate = Ongoing;
				state = Ongoing;
			}
			else if (state == Ongoing) {
				Vector3 a = nodes[currentNode - 1];
				Vector3 b = nodes[currentNode];
				Vector3 t = b - GetTransform().GetPosition();
				Vector3 dir = b - a;
				l1 = (getTarget1()->GetTransform().GetPosition() - GetTransform().GetPosition()).Length();
				if (getTarget2()!=NULL) {
					l2 = (getTarget2()->GetTransform().GetPosition() - GetTransform().GetPosition()).Length();
				}
				if ( l1> 20.0f && getTarget2() == NULL || l1> 20.0f && l2 > 20.0f) {
					Vector3 dis = b - GetTransform().GetPosition();
					GetPhysicsObject()->AddForce(dis.Normalised() * 2.5f);
					Debug::DrawLine(b, Vector3(0, 10, 0) + b, Vector4(1, 0, 0, 1));
					if ((b - GetTransform().GetPosition()).Length() < 1.5f) {
						currentNode++;
					}
					int l = (GetTransform().GetPosition() - nodes[currentNode]).Length();
					if (l > 15.0f) {
						GetTransform().SetPosition(nodes[currentNode]);
					}
				}
				else if (l1 < 20.0f || l2 < 20.0f) {
					std::cout << "Found target\n";
					currentstate = Success;
					state = Success;
				}
				if (currentNode == nodes.size()-1) {
					GetTransform().SetPosition(nodes[0]);
					currentNode = 1;
				}	
			}
			return state; //will be ’ongoing ’ until success
		}
	);

	BehaviourAction* Attack = new BehaviourAction("Attack goat\n",
		[&](float dt, BehaviourState state)->BehaviourState {
			l1 = (getTarget1()->GetTransform().GetPosition() - GetTransform().GetPosition()).Length();
			Vector3 dir;
			if (getTarget2()) {
				l2 = (getTarget2()->GetTransform().GetPosition() - GetTransform().GetPosition()).Length();
				
				if (l1 < l2) {
					dir = getTarget1()->GetTransform().GetPosition() - GetTransform().GetPosition();
					a = getTarget1();
				}
				else if (l2 < l1) {
					dir = getTarget2()->GetTransform().GetPosition() - GetTransform().GetPosition();
					a = getTarget2();
				}
			}
			else if (!getTarget2()) {
				dir = getTarget1()->GetTransform().GetPosition() - GetTransform().GetPosition();
				a = getTarget1();
			}
			if (state == Initialise) {
				currentstate = Ongoing;
				state = Ongoing;
			}
			else if (state == Ongoing) {
				GetPhysicsObject()->AddForce(dir*Vector3(1,0,1) * 10.0f);
				l1 = (getTarget1()->GetTransform().GetPosition() - GetTransform().GetPosition()).Length();
				if (getTarget2()) {
					l2 = (getTarget2()->GetTransform().GetPosition() - GetTransform().GetPosition()).Length();
						GetTransform().SetOrientation(-(a->GetTransform().GetOrientation()));
					if (l1 < 5.0f || l2 < 5.0f) {
						std::cout << "attacked\n";
						a->GetTransform().SetPosition(Vector3(0, -10, -350));
						currentstate = Success;
						state = Success;
					}
				}
				else {
					if (l1 < 5.0f) {
						std::cout << "attacked\n";
						getTarget1()->GetTransform().SetPosition(Vector3(0, -10, -350));
						currentstate = Success;
						state = Success;
					}
				}
			}
			return state; //will be ’ongoing ’ until success
		}
	);
	BehaviourAction* Rest = new BehaviourAction("resting\n",
		[&](float dt, BehaviourState state)->BehaviourState {
			float m = GetPhysicsObject()->GetInverseMass();
			if (state == Initialise) {
				std::cout << "temporary rest\n";
				restTime = 100.0f;
				currentstate = Ongoing;
				state = Ongoing;
			}
			else if (state == Ongoing) {
				std::cout << "temporary rest\n";
				
				restTime -= dt;
				if (restTime <= 0) {
					std::cout << "rested\n";
					currentstate = Success;
					std::cout << "next\n";
					GetTransform().SetPosition(nodes[currentNode]);
					state =  Success;
				}
			}
			return state;
		}
	);
	sequence =
		new BehaviourSequence("Patrol Sequence");
	sequence->AddChild(Patrol);
	sequence->AddChild(Attack);
	sequence->AddChild(Rest);

	
	BehaviourState state = Ongoing;
}
void BTreeObject::Update(float dt) {
	if (sequence->Execute(dt) == Success) {
		sequence->Reset();
	}

}
