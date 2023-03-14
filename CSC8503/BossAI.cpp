#include "BossAI.h"
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

BossAI::~BossAI() {
	delete rootSequence;
}

BossAI::BossAI(GameWorld* world, vector <Vector3 > testNodes) :GameObject(world) {
	nodes = testNodes; // getPathNodes
	currentNode = 1;
	float t;
	float restTime;
	GameObject* a;
	float l1;
	float l2;

	BehaviourAction* walkAct = new BehaviourAction("Patrolling ", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			std::cout << " patrolling !\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {

			// choose between four points on the map to patrol 
			
			int patrolPoints = rand() % 3;

			switch (patrolPoints)
			{
				case 0:
					WalkPath(Vector3(-10, 0, 30));
					break;
				case 1:
					WalkPath(Vector3(-20, 0, 30));
					break;
				case 2: 
					WalkPath(Vector3(-70, 0, 30));
					break;
				case 3:
					WalkPath(Vector3(-50, 0, 30));
					break;
			default:
				break;
			}

			// if the player is within the maze return true.

			if (SeenPlayer()) {
				std::cout << " Player in View!\n";
				state = Success;
			}
		}
		return state; // will be ’ongoing ’ until success
		});

	BehaviourAction* setRangeToTargetAct = new BehaviourAction("Set Range To player", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
		
			state = Ongoing;
		}
		else if (state == Ongoing) {

			// Get the distance value betwen the player and AI
			float currentDistance = (Vector3( GetPhysicsObject()->getTransform().getPosition()) - getTarget1()->GetPhysicsObject()->getTransform().getPosition()).Length();

			//set appropriate attack range
			currentDistance < 50 ? currentDistance < 10 ? range = closeRange : range = midRange : range = farRange;
			std::cout << " Set Attack Range";
			state = Success;
			
		}
		return state; 
		});

	BehaviourAction* stopAgentMovementAct = new BehaviourAction("Stop AI Movement", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
		
			// stop walk animation and get the direction vector to player 
			Vector3 dirVec = getTarget1()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition();
			float distance = VectorMagnitude(dirVec.x, dirVec.y, dirVec.z);
			 //normalised dir vector to player
			Vector3 normDirVec = dirVec / distance;

			state = Success;
		}
		return state;
		});

	BehaviourAction* moveTowardPlayerAct = new BehaviourAction("Running To player", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			// stop walk animation and get the direction vector to player 
			Vector3 dirVec = getTarget1()->GetPhysicsObject()->getTransform().getPosition() - GetPhysicsObject()->getTransform().getPosition();
			float distance = VectorMagnitude(dirVec.x, dirVec.y, dirVec.z);

			// normalised dir vector to player
			Vector3 normDirVec = dirVec / distance;

			/*nodeIndex = 0;
			newNodes.clear();
			destNotArrived = true;

			walkToPlayer = true;*/
			// set timer 

			timeLimit = 5;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			
			timeLimit -= dt;
			
			// run to player for a limited time
			WalkPath(getTarget1()->GetPhysicsObject()->getTransform().getPosition());

			if (timeLimit < 0) {

				state = Success;
			}
		}
		return state;
		});

	BehaviourAction* strafeAroundPlayerAct = new BehaviourAction("Get out of here!", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			state = Ongoing;
		}
		else if (state == Ongoing) {
			// move the player outside the maze 
			StrafeAroundPlayer(getTarget1()->GetPhysicsObject()->getTransform().getPosition());

			state = Success;
		}
		return state;
		});

	BehaviourAction* setAttackAnimAct = new BehaviourAction("Get out of here!", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {

			state = Ongoing;
		}
		else if (state == Ongoing) {
			
			switch (range) {
			case 0:
				
				break;
			case 1:
				break;
			case 2:
				break;
			}
			if ()
			{
				state = Success;
			}
		
		}
		return state;
		});

	BehaviourAction* playAttackAnimAct = new BehaviourAction("Get out of here!", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			state = Ongoing;
		}
		else if (state == Ongoing) {
			


			state = Success;
		}
		return state;
		});

	BehaviourAction* checkRangeAndTimeAct = new BehaviourAction("Get out of here!", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			state = Ongoing;
		}
		else if (state == Ongoing) {
			
			state = Success;
		}
		return state;
		});


	//root layer
	rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(patrolSequence);
	rootSequence->AddChild(attackSequence);

	//first layer 
	patrolSequence = new BehaviourSequence("Patrol Sequence");
	patrolSequence->AddChild(walkAct);
	patrolSequence->AddChild(seenPlayerSequence);

	attackSequence = new BehaviourSequence("Attack Sequence");
	attackSequence->AddChild(setRangeToTargetAct);
	attackSequence->AddChild(rangeForAttackSelector);
	attackSequence->AddChild(inRangeToTargetSequence);

	//second layer 
	seenPlayerSequence = new BehaviourSequence("Seen Player Sequence");
	seenPlayerSequence->AddChild(stopAgentMovementAct);
	seenPlayerSequence->AddChild(moveSelector);

	rangeForAttackSelector = new BehaviourSelector("Select Attack On Range");
	rangeForAttackSelector->AddChild(midRangeSequence);
	rangeForAttackSelector->AddChild(farRangeSequence);
	rangeForAttackSelector->AddChild(closeRangeSequence);

	inRangeToTargetSequence = new BehaviourSequence("In Range To Attack");
	inRangeToTargetSequence->AddChild(randomAttackSetSequence);


	//third layer
	moveSelector = new BehaviourSelector("Select Move");
	moveSelector->AddChild(moveTimerSequence);
	moveSelector->AddChild(strafeAroundPlayerAct);

	midRangeSequence = new BehaviourSequence("Medium Range Sequence");
	midRangeSequence->AddChild(setAttackAnimAct);
	midRangeSequence->AddChild(playAttackAnimAct);

	farRangeSequence = new BehaviourSequence("Medium Range Sequence");
	farRangeSequence->AddChild(setAttackAnimAct);
	farRangeSequence->AddChild(playAttackAnimAct);

	closeRangeSequence = new BehaviourSequence("Medium Range Sequence");
	closeRangeSequence->AddChild(closeMoveSelector);
	closeRangeSequence->AddChild(randomAttackSetSequence);

	//fourth layer
	closeMoveSelector = new BehaviourSelector("Close Move Selector");
	closeMoveSelector->AddChild(moveTimerSequence);
	closeMoveSelector->AddChild(strafeAroundPlayerAct);

	randomAttackSetSequence = new BehaviourSequence("Set Random Attack Set");
	randomAttackSetSequence->AddChild(setAttackAnimAct);
	randomAttackSetSequence->AddChild(playAttackAnimAct);

	//fifTHI
	moveTimerSequence = new BehaviourSequence("Move To Player Sequence");
	moveTimerSequence->AddChild(moveTowardPlayerAct);
	moveTimerSequence->AddChild(checkRangeAndTimeAct);

	BehaviourState state = Ongoing;
}

void BossAI::Update(float dt) {
	if (rootSequence->Execute(dt) == Success) {
		rootSequence->Reset();
	}

}

void NCL::CSC8503::BossAI::WalkPath(NCL::Maths::Vector3 position)
{

}


bool NCL::CSC8503::BossAI::SeenPlayer() // create a wedge volume from the perspective of boss. Only Check if the player is within it
{



	return false;
}

void NCL::CSC8503::BossAI::StrafeAroundPlayer(NCL::Maths::Vector3 position)
{
	// strafe around player 
	// if it encounters an obstacle on the strafe path i.e. radius, move back until it meets another obstacle. repeat 

	


}

float VectorMagnitude(int x, int y, int z)
{
	// Stores the sum of squares
	  // of coordinates of a vector
	int sum = x * x + y * y + z * z;

	// Return the magnitude
	return sqrt(sum);
}