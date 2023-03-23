#include "BossAI.h"
#include <reactphysics3d/reactphysics3d.h>

#include "Window.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include <Debug.cpp>
#include "RenderObject.h"
#include "NavigationGrid.h"
#include "NavigationMesh.h"
#include <cmath>

using namespace NCL;
using namespace CSC8503;


BossAI::BossAI(GameWorld* world, vector <Vector3 > mapNodes) :GameObject(world) {
	
	health = 100;
	
	pathNodes = mapNodes; // getPathNodes
	currentNode = 1;
	nodeIndex = 1;
	rNum = 0;

	walkOrAttack = false;

	inTime = true;
	dest1 = Vector3(45, 2, 20);
	dest2 = Vector3(75, 2, 40);
	dest3 = Vector3(50, 2, 30);
	dest4 = Vector3(40, 2, 25);
	timeLimit = 0;
	attackSelect = 0;
	
	aiWalkAnim = new MeshAnimation("botWalk.anm");
	aiRunAnim = new MeshAnimation("BotRun.anm");
	aicloseAttackAnim = new MeshAnimation("BotCloseAttack.anm");
	aiFarAttackAnim = new MeshAnimation("BotJump.anm");
	aiMidAttackAnim = new MeshAnimation("BotMidAttack.anm");
	aiRightStrafeAnim = new MeshAnimation("BotRightStrafe.anm");
	aiLeftStrafeAnim = new MeshAnimation("BotLeftStrafe.anm");
	aiDamaged = new MeshAnimation("BotHitReaction.anm");


	height = 10;
	AngThres = 70;
	outerRadius = 15;
	innerRadius = 2;
	
	CreateBehaviourTree();
}

BossAI::~BossAI() {
	delete rootSequence;
	delete patrolSequence;
	delete attackSequence;
	delete rangeForAttackSelector;
	delete moveSelector;
	delete strafeBehaviour;
	delete runThenAttackSequence;

	delete aiWalkAnim;
	delete aiRunAnim ;
	delete aicloseAttackAnim;
	delete aiMidAttackAnim;
	delete aiFarAttackAnim ;
	delete aiRightStrafeAnim ;
	delete aiLeftStrafeAnim ;
	delete aiDamaged;
}

void BossAI::UpdateBoss(float dt, NCL::Maths::Vector3& playerPos) {

	currPlayerPos = playerPos;
	frameTime -= dt;

	//currentstate  = Ongoing;
	currentstate = rootSequence->Execute(dt);

	if (currentstate == Success || currentstate == Failure) {
		rootSequence->Reset();
	}
}

void NCL::CSC8503::BossAI::Update(float dt)
{
	BehaviourState state = Ongoing;
	state = rootSequence->Execute(dt);

	if (state == Success || state == Failure) {
		rootSequence->Reset();
	}
}

void NCL::CSC8503::BossAI::CreateBehaviourTree()
{
	BehaviourAction* walkAct = new BehaviourAction("Patrolling ", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			std::cout << " patrolling !\n";
			state = Ongoing;


		}
		else if (state == Ongoing) {

			UpdateAnim(this,  aiWalkAnim, frameTime, currentFrame);
			
			// choose between four points on the map to patrol 
			int patrolPoints = rand() % 1;

			switch (patrolPoints)
			{
				case 0:
					WalkPath(dest1);
					break;
				case 1:
					WalkPath(dest2);
					break;
				//case 2:
					//WalkPath(dest3);
					//break;
				//case 3:
					//WalkPath(dest4);
					//break;
			default:
				break;
			}


		 SetRotationToPlayer();

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
			float currentDistance = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - currPlayerPos).Length();

			//set appropriate attack range
			currentDistance < 20 ? walkOrAttack = true: walkOrAttack = false;
			//std::cout << " Set Attack Range";
			state = Success;

		}
	return state;
		});


	BehaviourAction* strafeLeftAroundPlayerAct = new BehaviourAction("Left", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			// Calculate the vector from the AI to the player
			Vector3 playerToAI = GetPhysicsObject()->getTransform().getPosition() - reactphysics3d::Vector3(currPlayerPos.x, currPlayerPos.y, currPlayerPos.z);

			// Calculate a vector perpendicular to the playerToAI vector
			Vector3 strafeVector = Vector3(-playerToAI.z, 0, playerToAI.x).Normalised();

			// Move the AI in the direction of the strafe vector
			this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(strafeVector.x, strafeVector.y, strafeVector.z) * 5);

			state = Ongoing;


		}
		else if (state == Ongoing) {


			UpdateAnim(this, aiLeftStrafeAnim, frameTime, currentFrame);

			// Move the AI left for a set amount of time
			if (strafeTime > 0) {
				strafeTime -= dt;
				return Ongoing;
			}

			if (strafeTime < 0) {
				state = Success;
				strafeTime = 3;
			}
		}
	return state;
		});

	BehaviourAction* strafeRightAroundPlayerAct = new BehaviourAction("Right", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			// Calculate the vector from the AI to the player
			Vector3 playerToAI = GetPhysicsObject()->getTransform().getPosition() - reactphysics3d::Vector3(currPlayerPos.x, currPlayerPos.y, currPlayerPos.z);

			// Calculate a vector perpendicular to the playerToAI vector
			Vector3 strafeVector = Vector3(playerToAI.z, 0, -playerToAI.x).Normalised();

			// Move the AI in the direction of the strafe vector
			this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(strafeVector.x, strafeVector.y, strafeVector.z) * 5);

			state = Ongoing;
		}
		else if (state == Ongoing) {

			UpdateAnim(this, aiRightStrafeAnim, frameTime, currentFrame);
			// Move the AI left for a set amount of time
			if (strafeTime > 0) {
				strafeTime -= dt;
				return Ongoing;
			}

			if (strafeTime < 0) {
				state = Success;
				strafeTime = 3;
			}

		}
	return state;
		});

	BehaviourAction* runToPlayerAttack = new BehaviourAction("Running to Player", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			state = Ongoing;
			std::cout << "Walking to Player!\n";
		}
		else if (state == Ongoing) {

			float initialSpeed = 20.0f;
			float stopDist = 1.0f;
			if (walkOrAttack) {
				std::cout << "Moving to player!\n";
				UpdateAnim(this, aiRunAnim, frameTime, currentFrame);
				Vector3 dir = (currPlayerPos - this->GetPhysicsObject()->getTransform().getPosition()).Normalised();
			
				float currentDist = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - currPlayerPos).Length();

				// Interpolate the movement speed based on the current distance to the player
				movSpeed = initialSpeed * std::max((currentDist - stopDist) / (initialSpeed - 2), 0.0f);

				// Apply the movement force
				this->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(dir.x, dir.y, dir.z) * movSpeed);

				std::cout << "Distance to player: " << currentDist << std::endl;
				std::cout << "Movement speed: " << movSpeed << std::endl;

				SetRotationToPlayer();
				// Check if the AI has reached the player
				if (currentDist < 5) {
					
					state = Success;
					std::cout << "Attack Success!\n";
				}
			}
			else {
				state = Failure;
			}
		}
		return state;
	});

	BehaviourAction* farAttackAnimAct = new BehaviourAction("Far Attack", [&](float dt, BehaviourState state) -> BehaviourState {

		if (state == Initialise) {
			std::cout << "jumping" << std::endl;
			Vector3 aiPos = this->GetPhysicsObject()->getTransform().getPosition();
			Vector3 aiVelocity = GetPhysicsObject()->getLinearVelocity();
			Vector3 playerToAI = currPlayerPos - aiPos;

			// Calculate the jump trajectory
			float jumpHeight = 10.0f;
			float jumpTime = 1.0f;
			float jumpDistance =  (Vector3(GetPhysicsObject()->getTransform().getPosition()) - currPlayerPos).Length();
			float gravity = 9.81f;

			float initialVelocity = sqrtf((2 * jumpHeight * gravity) / jumpTime);// Calculate the initial velocity of the jump
			Vector3 jumpDirection = playerToAI.Normalised() + Vector3(0, 1, 0);// Calculate the direction of the jump

			jumpDirection.Normalise();

			Vector3 jumpVelocity = jumpDirection * initialVelocity;	// Calculate the velocity of the jump

			this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(jumpVelocity.x, jumpVelocity.y, jumpVelocity.z));// Set the linear velocity of the jump

			state = Ongoing;
		}
		else if (state == Ongoing) {

			SetRotationToPlayer();

			UpdateAnim(this, aiFarAttackAnim, frameTime, currentFrame);

			if (GetPhysicsObject()->getLinearVelocity().y <= 0.0f) {
				std::cout << "jump sucessful" << std::endl;
				state = Success;
			}
		}
		return state;
	});

	BehaviourAction* closeAttackAnimAct = new BehaviourAction("Close Attack", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			state = Ongoing;
			std::cout << " close Attack !\n";

			attackSelect = rand() % 1;
		}
		else if (state == Ongoing) {
			if (range == 0) {

				SetRotationToPlayer();
				
				Vector3 dir = (currPlayerPos - this->GetPhysicsObject()->getTransform().getPosition()).Normalised();

				// Calculate the distance to the player
				float currentDist = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - currPlayerPos).Length();

				// Apply the movement force
				this->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(dir.x, 0, dir.z)* 10);

				attackSelect == 0 ? UpdateAnim(this, aicloseAttackAnim, frameTime, currentFrame) : UpdateAnim(this, aiMidAttackAnim, frameTime, currentFrame);

				// Check if the AI has reached the player
				if (currentDist < 1) {
					state = Success;
					std::cout << "Attack Success!\n";

				}
			}
			else
				state = Failure;
		}
		return state;
	});

	strafeBehaviour = new BehaviourSequence("strafing Sequence");
	strafeBehaviour->AddChild(strafeLeftAroundPlayerAct);
	strafeBehaviour->AddChild(strafeRightAroundPlayerAct);

	runThenAttackSequence = new BehaviourSequence("Run/Attack Sequence");
	runThenAttackSequence->AddChild(runToPlayerAttack);
	runThenAttackSequence->AddChild(closeAttackAnimAct);


	rangeForAttackSelector = new BehaviourSelector("Select Attack Range");
	rangeForAttackSelector->AddChild(runThenAttackSequence);
	rangeForAttackSelector->AddChild(farAttackAnimAct);

	patrolSequence = new BehaviourSequence("Patrol Sequence");
	patrolSequence->AddChild(walkAct);
	patrolSequence->AddChild(strafeBehaviour);

	attackSequence = new BehaviourSequence("Attack Sequence");
	attackSequence->AddChild(setRangeToTargetAct);
	attackSequence->AddChild(rangeForAttackSelector);

	rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(patrolSequence);
	rootSequence->AddChild(attackSequence);
}

bool NCL::CSC8503::BossAI::SeenPlayer() // create a wedge volume from the perspective of boss. Only Check if the player is within it
{
	DrawWedgeVolume(height, AngThres, outerRadius, innerRadius);

	 // Get direction to target
	Vector3 dirToTargetWorld = (currPlayerPos - this->GetPhysicsObject()->getTransform().getPosition());

	// Convert direction to local space
	Vector3 vecToTarget = this->GetPhysicsObject()->getLocalVector(reactphysics3d::Vector3(dirToTargetWorld.x, dirToTargetWorld.y, dirToTargetWorld.z));

	vecToTarget.y += 2;

	// Check if the target is outside the height range
	if (vecToTarget.y < 0 || vecToTarget.y > height) {
		return false;
	}

	// Calculate wedge boundaries based on parameters
	Vector3 forward = this->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, -1);
	Vector3 right = this->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(1, 0, 0);
	float halfAngle = AngThres / 2.0f;
	float innerDist = innerRadius / std::cos(halfAngle * PI / 180.0f);
	float outerDist = outerRadius / std::cos(halfAngle * PI / 180.0f);
	float innerWidth = innerDist * std::tan(halfAngle * PI / 180.0f);
	float outerWidth = outerDist * std::tan(halfAngle * PI / 180.0f);

	// Calculate projection of the target vector onto the forward and right vectors
	float projDistance = Vector3::Projection(vecToTarget, forward).Length();
	float projWidth = Vector3::Projection(vecToTarget, right).Length();

	// Check if the target is within the wedge boundaries
	if (projDistance > innerDist && projDistance <= outerDist &&
		projWidth > innerWidth && projWidth <= outerWidth) {
		return true;
	}

	return false;
}

void NCL::CSC8503::BossAI::DrawWedgeVolume( float height, float AngThres, float outerRadius, float innerRadius)
{
	// get AI position and orientation
	Vector3 aiPos = this->GetPhysicsObject()->getTransform().getPosition();
	Quaternion aiRot = this->GetPhysicsObject()->getTransform().getOrientation();

	// calculate vectors for wedge boundaries
	Vector3 forward = aiRot * Vector3(0, 0, -1);
	Vector3 right = aiRot * Vector3(1, 0, 0);
	Vector3 up = aiRot * Vector3(0, 1, 0);

	// calculate wedge boundaries based on parameters
	float halfAngle = AngThres / 2.0f;
	float innerDist = innerRadius / std::cos(halfAngle * PI / 180.0f);
	float outerDist = outerRadius / std::cos(halfAngle * PI / 180.0f);
	float innerWidth = innerDist * std::tan(halfAngle * PI / 180.0f);
	float outerWidth = outerDist * std::tan(halfAngle * PI / 180.0f);
	float minHeight = aiPos.y;
	float maxHeight = aiPos.y + height;

	// draw wedge boundaries
	Vector3 start, end;

	start = aiPos + forward * innerDist + right * innerWidth;
	end = aiPos + forward * outerDist + right * outerWidth;
	Debug::DrawLine(start, end, Vector4(1, 0, 0, 1)); // inner right

	start = aiPos + forward * outerDist + right * outerWidth;
	Debug::DrawLine(end, start + up * (maxHeight - minHeight), Vector4(1, 0, 0, 1)); // outer top

	start = aiPos + forward * outerDist + right * outerWidth;
	end = aiPos + forward * outerDist - right * outerWidth;
	Debug::DrawLine(start + up * (maxHeight - minHeight), end + up * (maxHeight - minHeight), Vector4(1, 0, 0, 1)); // outer back

	start = aiPos + forward * outerDist - right * outerWidth;
	Debug::DrawLine(end + up * (maxHeight - minHeight), start, Vector4(1, 0, 0, 1)); // outer left

	start = aiPos + forward * innerDist - right * innerWidth;
	end = aiPos + forward * outerDist - right * outerWidth;
	Debug::DrawLine(start, end, Vector4(1, 0, 0, 1)); // inner left

	start = aiPos + forward * innerDist - right * innerWidth;
	Debug::DrawLine(end, start + up * (maxHeight - minHeight), Vector4(1, 0, 0, 1)); // inner top

	start = aiPos + forward * innerDist + right * innerWidth;
	Debug::DrawLine(start, start + up * (maxHeight - minHeight), Vector4(1, 0, 0, 1)); // inner back

	Debug::DrawLine(aiPos + up * minHeight, aiPos + up * maxHeight, Vector4(1, 0, 0, 1)); // height
}


void NCL::CSC8503::BossAI::CreatePath(Vector3& targetDist)
{
	NavigationGrid grid("TestGrid5.txt");
	NavigationPath outPath;

	float x = targetDist.x;
	float z = targetDist.z;

	Vector3 endPos = Vector3(x, 0, z);

	bool found = grid.FindPath(GetPhysicsObject()->getTransform().getPosition(), endPos, outPath);
	foundPath = found;

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		pathNodes.push_back(pos);
	}
}

void NCL::CSC8503::BossAI::DisplayPath()
{
	for (int i = 1; i < pathNodes.size(); ++i) {
		Vector3 a = pathNodes[i - 1];
		Vector3 b = pathNodes[i];
		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}

void NCL::CSC8503::BossAI::WalkPath(Vector3& destination)
{
	if (pathNodes.size() == 0) {
		CreatePath(destination);
		destNotArrived = true;
	}

	if (!foundPath) {
		CreatePath(destination);
	}

	if (walkToPlayer && nodeIndex == pathNodes.size()) {
		nodeIndex = 0;
		pathNodes.clear();
		destNotArrived = true;
		return;
	}
	else if (nodeIndex == pathNodes.size())
	{
		destNotArrived = false;
		nodeIndex = 0;
		pathNodes.clear();
		rNum == 0 ? rNum = 1 : rNum = 0;
		return;
	}

	float distToNode = (Vector3(this->GetPhysicsObject()->getTransform().getPosition()) - pathNodes[nodeIndex]).Length();

	if (distToNode >= 2.0f && destNotArrived) {
		float x = pathNodes[nodeIndex].x > this->GetPhysicsObject()->getTransform().getPosition().x ? 10 : -10;
		float z = pathNodes[nodeIndex].z > this->GetPhysicsObject()->getTransform().getPosition().z ? 10 : -10;
		this->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(x, 0, z));
	}

	if (distToNode <= 4.0f && destNotArrived)
	{
		nodeIndex += 1;
	}
}

float NCL::CSC8503::BossAI::VectorMagnitude(float x, float y, float z)
{
	// Stores the sum of squares of coordinates of a vector
	float sum = x * x + y * y + z * z;

	// Return the magnitude
	return sqrt(sum);
}

double NCL::CSC8503::BossAI::DegreesToRadian(double degrees)
{
	return degrees * 3.14159265358 / 180.0;
}

void NCL::CSC8503::BossAI::SetRotationToPlayer()
{

	Vector3 aiPos = this->GetPhysicsObject()->getTransform().getPosition();

	Vector3 currentVelocity = Vector3(this->GetPhysicsObject()->getLinearVelocity());
	float theta = atan2(currentVelocity.z, currentVelocity.x) * (180 / PI);
	Quaternion aiTargetRotation = Quaternion(Matrix4::Rotation(-theta - 90, Vector3(0, 1, 0)));
	Quaternion aiStartRotation = Quaternion(this->GetPhysicsObject()->getTransform().getOrientation());
	Quaternion  aiRealRotation = Quaternion::Lerp(aiStartRotation, aiTargetRotation, 0.5f);

	// Set the orientation of the AI to look at the player

	reactphysics3d::Transform newTransform = reactphysics3d::Transform(reactphysics3d::Vector3(aiPos.x, aiPos.y, aiPos.z),
		reactphysics3d::Quaternion(aiRealRotation.x, aiRealRotation.y, aiRealRotation.z, aiRealRotation.w));
	this->GetPhysicsObject()->setTransform(newTransform);
}

void NCL::CSC8503::BossAI::UpdateAnim(BossAI* p, MeshAnimation* anim, float& ftime, int& cframe)
{
	while (ftime < 0.0f) {
		cframe = (cframe + 1) % anim->GetFrameCount();
		ftime += 1.0f / anim->GetFrameRate();
	}
	DrawAnim(p, anim, cframe);
}

void NCL::CSC8503::BossAI::DrawAnim(BossAI* p, MeshAnimation* anim, int& cframe)
{
	const Matrix4* invBindPose = p->GetRenderObject()->GetMesh()->GetInverseBindPose().data();
	const Matrix4* frameData = anim->GetJointData(cframe % anim->GetFrameCount());
	vector <Matrix4 > tempMatrices;
	for (unsigned int i = 0; i < p->GetRenderObject()->GetMesh()->GetJointCount(); ++i) {
		tempMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}
	p->GetRenderObject()->SetFrameMatrices(tempMatrices);
}




