#include "BTreeObject.h"
#include <reactphysics3d/reactphysics3d.h>
#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "HierarchicalState.h"
#include "Window.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include <Debug.cpp>


using namespace NCL;
using namespace CSC8503;

BTreeObject::~BTreeObject() {
	delete rootSequence;
}

BTreeObject::BTreeObject(GameWorld* world, vector <Vector3 > testNodes):GameObject(world) {
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
				Vector3 t = b - GetPhysicsObject()->getTransform().getPosition();
				Vector3 dir = b - a;
				l1 = Vector3(getTarget1()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition()).Length();
				if (getTarget2()!=NULL) {
					l2 = Vector3(getTarget2()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition()).Length();
				}
				if ( l1> 20.0f && getTarget2() == NULL || l1> 20.0f && l2 > 20.0f) {
					Vector3 dis = b - GetPhysicsObject()->getTransform().getPosition();
					Vector3 force = dis.Normalised() * 2.5f;
					GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
					Debug::DrawLine(b, Vector3(0, 10, 0) + b, Vector4(1, 0, 0, 1));
					if (Vector3(b - GetPhysicsObject()->getTransform().getPosition()).Length() < 1.5f) {
						currentNode++;
					}
					int l = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - nodes[currentNode]).Length();
					if (l > 15.0f) {
						Vector3 position = nodes[currentNode];
						GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(position.x, position.y, position.z), GetPhysicsObject()->getTransform().getOrientation()));
					}
				}
				else if (l1 < 20.0f || l2 < 20.0f) {
					std::cout << "Found target\n";
					currentstate = Success;
					state = Success;
				}
				if (currentNode == nodes.size()-1) {
					Vector3 position = nodes[0];
					GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(position.x, position.y, position.z), GetPhysicsObject()->getTransform().getOrientation()));
					currentNode = 1;
				}	
			}
			return state; //will be ’ongoing ’ until success
		}
	);

	BehaviourAction* Attack = new BehaviourAction("Attack goat\n",
		[&](float dt, BehaviourState state)->BehaviourState {
			l1 = Vector3(getTarget1()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition()).Length();
			Vector3 dir;
			if (getTarget2()) {
				l2 = Vector3(getTarget2()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition()).Length();

				if (l1 < l2) {
					dir = getTarget1()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition();
					a = getTarget1();
				}
				else if (l2 < l1) {
					dir = getTarget2()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition();
					a = getTarget2();
				}
			}
			else if (!getTarget2()) {
				dir = getTarget1()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition();
				a = getTarget1();
			}
			if (state == Initialise) {
				currentstate = Ongoing;
				state = Ongoing;
			}
			else if (state == Ongoing) {
				Vector3 force = dir * Vector3(1, 0, 1) * 10.0f;
				GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(force.x, force.y, force.z));
				l1 = Vector3(getTarget1()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition()).Length();
				if (getTarget2()) {
					l2 = Vector3(getTarget2()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition()).Length();
					Quaternion orientation = -(Quaternion(a->GetPhysicsObject()->getTransform().getOrientation()));
					    GetPhysicsObject()->setTransform(reactphysics3d::Transform(GetPhysicsObject()->getTransform().getPosition(),  reactphysics3d::Quaternion(orientation.x, orientation.y, orientation.z, orientation.w)));
					if (l1 < 5.0f || l2 < 5.0f) {
						std::cout << "attacked\n";
						a->GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(0, -10, -350), a->GetPhysicsObject()->getTransform().getOrientation()));
						currentstate = Success;
						state = Success;
					}
				}
				else {
					if (l1 < 5.0f) {
						std::cout << "attacked\n";
						getTarget1()->GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(0, -10, -350), getTarget1()->GetPhysicsObject()->getTransform().getOrientation()));
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
			float m = 1.0f / GetPhysicsObject()->getMass();
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
					Vector3 position = nodes[currentNode];
					GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(position.x, position.y, position.z), GetPhysicsObject()->getTransform().getOrientation()));
					state =  Success;
				}
			}
			return state;
		}
	);



	rootSequence =new BehaviourSequence("Patrol Sequence");
	rootSequence->AddChild(Patrol);
	rootSequence->AddChild(Attack);
	rootSequence->AddChild(Rest);

	





	BehaviourState state = Ongoing;

}
void BTreeObject::Update(float dt) {
	if (rootSequence->Execute(dt) == Success) {
		rootSequence->Reset();
	}

}
