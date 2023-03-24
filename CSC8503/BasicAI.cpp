#include "BasicAI.h"
#include <reactphysics3d/reactphysics3d.h>
#include "Window.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "Debug.cpp"
#include "RenderObject.h"
#include "NavigationGrid.h"
#include "NavigationMesh.h"
#include <cmath>
#include "PlayerObject.h"
#include "Projectile.h"
#include "MeshAnimation.h"


using namespace NCL;
using namespace CSC8503;

BasicAI::BasicAI(GameWorld* world, vector <Vector3 > mapNodes, std::string aiName) :GameObject(world) {
	name = aiName;

	health = 100;
	pathNodes = mapNodes; // getPathNodes
	currentNode = 1;
	nodeIndex = 1;
	rNum = 0;

	walkOrAttack = false;

	inTime = true;
	dest1 = Vector3(45, 2, 20);
	dest2 = Vector3(115, 2, 100);
	//dest3 = Vector3(115, 2, 30);
	timeLimit = 0;
	attackSelect = 0;

	aiWalkAnim = new MeshAnimation("AIWalking.anm");
	aiRunAnim = new MeshAnimation("AIRun.anm");
	aicloseAttackAnim = new MeshAnimation("AICloseAttack.anm");
	aiFarAttackAnim = new MeshAnimation("AIJump.anm");
	aiJumpBack = new MeshAnimation("Backflip.anm");
	aiFarAttackTwoAnim = new MeshAnimation("AIJumpAttack.anm");
	aiRightStrafeAnim = new MeshAnimation("AIRightStrafe.anm");
	aiLeftStrafeAnim = new MeshAnimation("AILeftStrafe.anm");
	//aiDamaged = new MeshAnimation("AIHitReaction.anm");
	
	blendedAnimm = BlendAnimation(aiFarAttackAnim, aiFarAttackTwoAnim, 0.2f);

	height = 10;
	AngThres = 70;
	outerRadius = 15;
	innerRadius = 1;

	CreateBehaviourTree();
}

BasicAI::~BasicAI() {
	delete rootSequence;
	delete patrolSequence;
	delete attackSequence;
	delete rangeForAttackSelector;
	delete moveSelector;
	delete strafeBehaviour;
	delete runThenAttackSequence;
	delete jumpSequence;

	delete aiWalkAnim;
	delete aiRunAnim;
	delete aicloseAttackAnim;
	delete aiFarAttackTwoAnim;
	delete aiFarAttackAnim;
	delete aiRightStrafeAnim;
	delete aiLeftStrafeAnim;
	delete aiDamaged;
	delete aiJumpBack;
	delete blendedAnimm;
}

void BasicAI::UpdateBoss(float dt, NCL::Maths::Vector3& playerPos) {

	currPlayerPos = playerPos;
	frameTime -= dt;

	//std::cout << currPlayerPos << std::endl;
	//currentstate  = Ongoing;
	currentstate = rootSequence->Execute(dt);

	if (currentstate == Success || currentstate == Failure) {
		rootSequence->Reset();
	}
}

void NCL::CSC8503::BasicAI::Update(float dt)
{
	BehaviourState state = Ongoing;
	state = rootSequence->Execute(dt);

	if (state == Success || state == Failure) {
		rootSequence->Reset();
	}
}

void NCL::CSC8503::BasicAI::CreateBehaviourTree()
{
	BehaviourAction* walkAct = new BehaviourAction("Patrolling ", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
		//	std::cout << " patrolling !\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {

			UpdateAnim(this, aiWalkAnim, frameTime, currentFrame);

			switch (rNum)
			{
			case 0:
				WalkPath(dest1);
				break;
			case 1:
				WalkPath(dest2);
				break;
		
			default:
				break;
			}
			SetRotationToPlayer();

			if (SeenPlayer()) {
				//std::cout << " Player in View!\n";
				state = Success;
			}
		}
		return state; 
		});

	BehaviourAction* setRangeToTargetAct = new BehaviourAction("Set Range To player", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			//std::cout << " Setting Range!\n";
			state = Ongoing;
		}
		else if (state == Ongoing) {

			float currentDistance = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - currPlayerPos).Length();
	
			reactphysics3d::Ray ray(this->GetPhysicsObject()->getTransform().getPosition() + this->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, 1), reactphysics3d::Vector3(currPlayerPos.x, currPlayerPos.y, currPlayerPos.z));
			SceneContactPoint* playerVisible = world->Raycast(ray);

			if (playerVisible->isHit) { 
				//std::cout << "hit";

				if (dynamic_cast<PlayerObject*>(playerVisible->object)) {
					//std::cout << " hit player" << std::endl;
					//std::cout << playerVisible->object->GetTag() << std::endl;
					currentDistance < 20 ? walkOrAttack = true : walkOrAttack = false;
					state = Success;
				}
				else {
					//std::cout << playerVisible->object->GetTag() << std::endl;
				//	std::cout << "fail hit" << std::endl;
					state = Failure;
				}
			}
			else {
				//std::cout << "no ray";
				delete playerVisible;
				state = Failure;
			}
		}
		return state;
	});


	BehaviourAction* strafeLeftAroundPlayerAct = new BehaviourAction("Left", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {

			std::cout << "Left Strafe!\n";
			
			Vector3 playerToAI = GetPhysicsObject()->getTransform().getPosition() - reactphysics3d::Vector3(currPlayerPos.x, currPlayerPos.y, currPlayerPos.z);
			Vector3 strafeVector = Vector3(-playerToAI.z, 0, playerToAI.x).Normalised();
			this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(strafeVector.x, 0, strafeVector.z) * 3.5f);
			state = Ongoing;
		}
		else if (state == Ongoing) {
			UpdateAnim(this, aiLeftStrafeAnim, frameTime, currentFrame);

			//SetRotationToPlayer();

			if (strafeTime > 0) {
				strafeTime -= dt;
				return Ongoing;
			}
			else {
				state = Success;
				strafeTime = 3;
			}
		}
		return state;
		});

	BehaviourAction* strafeRightAroundPlayerAct = new BehaviourAction("Right", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {

			std::cout << " Right Strafe!\n";
			
			Vector3 playerToAI = GetPhysicsObject()->getTransform().getPosition() - reactphysics3d::Vector3(currPlayerPos.x, currPlayerPos.y, currPlayerPos.z);
			Vector3 strafeVector = Vector3(playerToAI.z, 0, -playerToAI.x).Normalised();
			this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(strafeVector.x, 0, strafeVector.z) * 3.5f);
			state = Ongoing;
			strafeTime = 0;
		}
		else if (state == Ongoing) {

			UpdateAnim(this, aiRightStrafeAnim, frameTime, currentFrame);

		//	SetRotationToPlayer();
			
			if (strafeTime > 0) {
				strafeTime -= dt;
				return Ongoing;
			}else{
				state = Success;
			}
		}
		return state;
		});

	BehaviourAction* runToPlayerAttack = new BehaviourAction("Running to Player", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			state = Ongoing;
			//std::cout << "Walking to Player!\n";
		}
		else if (state == Ongoing) {

			float initialSpeed = 10.0f;
			float stopDist = 3.0f;
			if (walkOrAttack) {
				UpdateAnim(this, aiRunAnim, frameTime, currentFrame);
				Vector3 dir = (currPlayerPos - this->GetPhysicsObject()->getTransform().getPosition()).Normalised();

				float currentDist = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - currPlayerPos).Length();

				//Interpolate the movement speed based on the current distance to the player
				movSpeed = initialSpeed * std::max((currentDist - stopDist) / (initialSpeed - 2), 0.0f);

				this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(dir.x, dir.y, dir.z)* movSpeed);

				SetRotationToPlayer();
				if (currentDist < 5) {
					state = Success;
					//std::cout << "Near Player!\n";
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
			std::cout << "jumping from Rnage" << std::endl;
			Vector3 aiPos = this->GetPhysicsObject()->getTransform().getPosition();
			Vector3 aiVelocity = GetPhysicsObject()->getLinearVelocity();
			Vector3 playerToAI = currPlayerPos - aiPos;

			// Calculate the jump trajectory
			float jumpHeight = 10.0f;
			float jumpTime = 1.0f;
			float jumpDistance = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - currPlayerPos).Length();
			float gravity = 9.81f;

			float initialVelocity = sqrtf((2 * jumpHeight * gravity) / jumpTime);// Calculate the initial velocity of the jump
			Vector3 jumpDirection = playerToAI.Normalised() + Vector3(0, 1, 0);// Calculate the direction of the jump

			jumpDirection.Normalise();

			Vector3 jumpVelocity = jumpDirection * initialVelocity;	// Calculate the velocity of the jump
			strafeTime = 4;
			this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(jumpVelocity.x, jumpVelocity.y, jumpVelocity.z));// Set the linear velocity of the jump
			state = Ongoing;
		}
		else if (state == Ongoing) {
			if (!walkOrAttack) {
				SetRotationToPlayer();
				UpdateAnim(this, blendedAnimm, frameTime, currentFrame);
				Vector3 aiPos = this->GetPhysicsObject()->getTransform().getPosition();

				//std::cout << aiPos << std::endl;

				strafeTime -= dt;
				float landedYPos = this->GetPhysicsObject()->getTransform().getPosition().y;
				if (strafeTime <= 0 && landedYPos <= 4.0f) {
					return Success; // Return Success
				}
			}
			else {
				state = Success;
			}
		}
		return state;
	});

	BehaviourAction* checkLandedAct = new BehaviourAction("Land Check ", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			std::cout << "Check Landing " << std::endl;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			Vector3 aiVelocity = this->GetPhysicsObject()->getLinearVelocity();
			SetRotationToPlayer();
			if (aiVelocity.y <= 0.0f) {
				
			//	std::cout << "AI has landed" << std::endl;
				state = Success;
			}
			else {
				state = Failure;
			}
		}
	return state;
		});

	BehaviourAction* closeAttackAnimAct = new BehaviourAction("Close Attack", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			hitTime = 1.5f; // Set hit animation duration
			state = Ongoing;

		}
		else if (state == Ongoing) {
			if (range == 0) {
				UpdateAnim(this, aicloseAttackAnim, frameTime, currentFrame);
				hitTime -= dt;
				SetRotationToPlayer();
				Vector3 dir = (currPlayerPos - this->GetPhysicsObject()->getTransform().getPosition()).Normalised();
				float currentDist = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - currPlayerPos).Length();
				this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(dir.x, dir.y, dir.z) * 8);

				if (hitTime <= 0.0f) {
					state = Success; 
				}
			}
			else
				state = Failure;
		}
		return state;
	});

	BehaviourAction* dodgeAct = new BehaviourAction("Dodge Attack", [&](float dt, BehaviourState state) -> BehaviourState {
		static float timer = 0.0f;
		if (state == Initialise) {
			std::cout << "jumping Back" << std::endl;
			Vector3 aiPos = this->GetPhysicsObject()->getTransform().getPosition();
			Vector3 aiVelocity = GetPhysicsObject()->getLinearVelocity();
			Vector3 playerToAI = currPlayerPos - aiPos;

			float jumpHeight = 5.0f;
			float jumpTime = 1.0f;
			float jumpDistance = 5;
			float gravity = 9.81f;

			float initialVelocity = sqrtf((2 * jumpHeight * gravity) / jumpTime);
		
			Vector3 oppositeJumpDirection = -playerToAI.Normalised() + Vector3(0, 1, 0);
			oppositeJumpDirection.Normalise();

			Vector3 jumpVelocity = oppositeJumpDirection * initialVelocity;

			this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(jumpVelocity.x, jumpVelocity.y, jumpVelocity.z));
			timer = 0.0f;
			state = Ongoing;
		}
		else if (state == Ongoing) {
			UpdateAnim(this, aiJumpBack, frameTime, currentFrame);

			if(GetPhysicsObject()->getLinearVelocity().y <= 0.0f) {
				if (timer >= 2.0f) { // Stand still for 2 seconds
				//	std::cout << "Jump successful" << std::endl;
					state = Success;
				}
				else {
					timer += dt;
				}
			}
		else {
			state = Failure;
		}
		}
		return state;
	});

	BehaviourAction* goHomeAct = new BehaviourAction("Home Attack", [&](float dt, BehaviourState state) -> BehaviourState {
	if (state == Initialise) {
		state = Ongoing;
		nodeIndex = 0;
		pathNodes.clear();
		std::cout << "jumping Back" << std::endl;
	}
	else if (state == Ongoing) {
		UpdateAnim(this, aiRunAnim, frameTime, currentFrame);

		Vector3 dest = Vector3(10, 10, 10);
		WalkPath(dest);

		float distance = (Vector3(GetPhysicsObject()->getTransform().getPosition()) - dest).Length();
		if(distance < 10) {
			state = Success;
		}
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
	runThenAttackSequence->AddChild(dodgeAct);
	runThenAttackSequence->AddChild(goHomeAct);

	jumpSequence = new BehaviourSequence("Jump Sequence ");
	jumpSequence->AddChild(farAttackAnimAct);
	jumpSequence->AddChild(checkLandedAct);

	rangeForAttackSelector = new BehaviourSelector("Select Attack Range");
	rangeForAttackSelector->AddChild(jumpSequence);
	rangeForAttackSelector->AddChild(runThenAttackSequence);

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

bool NCL::CSC8503::BasicAI::SeenPlayer() // create a wedge volume from the perspective of boss. Only Check if the player is within it
{
	DrawWedgeVolume(height, AngThres, outerRadius, innerRadius);

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

void NCL::CSC8503::BasicAI::OnCollisionBegin(GameObject* otherObject)
{
	if (dynamic_cast<Projectile*>(otherObject)) {
		if (aiHealth > 0) {
			aiHealth -= 0.08f;
		}
		//	std::cout << "aihealth: " << aiHealth << std::endl;
	}

	if (aiHealth <= 0) {
		this->setActive(false);
		this->GetPhysicsObject()->setType(reactphysics3d::BodyType::STATIC);
	}

}

void NCL::CSC8503::BasicAI::DrawWedgeVolume(float height, float AngThres, float outerRadius, float innerRadius)
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


void NCL::CSC8503::BasicAI::CreatePath(Vector3& targetDist)
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

void NCL::CSC8503::BasicAI::DisplayPath()
{
	for (int i = 1; i < pathNodes.size(); ++i) {
		Vector3 a = pathNodes[i - 1];
		Vector3 b = pathNodes[i];
		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}

void NCL::CSC8503::BasicAI::WalkPath(Vector3& destination)
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

	Vector3 dir = (pathNodes[nodeIndex] - this->GetPhysicsObject()->getTransform().getPosition()).Normalised();
	float distToNode = (Vector3(this->GetPhysicsObject()->getTransform().getPosition()) - pathNodes[nodeIndex]).Length();

	if (distToNode >= 2.0f && destNotArrived) {
		this->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(dir.x, dir.y, dir.z) * 4);
	}

	if (distToNode <= 4.0f && destNotArrived)
	{
		nodeIndex += 1;
	}
}

float NCL::CSC8503::BasicAI::VectorMagnitude(float x, float y, float z)
{
	// Stores the sum of squares of coordinates of a vector
	float sum = x * x + y * y + z * z;

	// Return the magnitude
	return sqrt(sum);
}

double NCL::CSC8503::BasicAI::DegreesToRadian(double degrees)
{
	return degrees * 3.14159265358 / 180.0;
}

void NCL::CSC8503::BasicAI::SetRotationToPlayer()
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

void NCL::CSC8503::BasicAI::UpdateAnim(BasicAI* p, MeshAnimation* anim, float& ftime, int& cframe)
{
	while (ftime < 0.0f) {
		cframe = (cframe + 1) % anim->GetFrameCount();
		ftime += 1.0f / anim->GetFrameRate();
	}
	DrawAnim(p, anim, cframe);
}

void NCL::CSC8503::BasicAI::DrawAnim(BasicAI* p, MeshAnimation* anim, int& cframe)
{
	const Matrix4* invBindPose = p->GetRenderObject()->GetMesh()->GetInverseBindPose().data();
	const Matrix4* frameData = anim->GetJointData(cframe % anim->GetFrameCount());
	vector <Matrix4 > tempMatrices;
	for (unsigned int i = 0; i < p->GetRenderObject()->GetMesh()->GetJointCount(); ++i) {
		tempMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}
	p->GetRenderObject()->SetFrameMatrices(tempMatrices);
}
MeshAnimation* NCL::CSC8503::BasicAI::BlendAnimation(MeshAnimation* anim1, MeshAnimation* anim2, float blendFactor)
{
	if (anim1->GetJointCount() != anim2->GetJointCount() || anim1->GetFrameCount() != anim2->GetFrameCount()) {
		//std::cout << "Error: Animations must have the same number of joints and frames to be blended!" << std::endl;
		//return nullptr;
	}

	unsigned int jointCount = anim1->GetJointCount();
	unsigned int frameCount = std::min(anim1->GetFrameCount(), anim2->GetFrameCount());
	float frameRate = anim1->GetFrameRate();

	std::vector<Matrix4> blendedFrames;
	blendedFrames.reserve(jointCount * frameCount);

	// Blend the joint transformations for each frame
	for (unsigned int frame = 0; frame < frameCount; ++frame) {
		const Matrix4* frameData1 = anim1->GetJointData(frame);
		const Matrix4* frameData2 = anim2->GetJointData(frame);

		for (unsigned int joint = 0; joint < jointCount; ++joint) {
			Matrix4 blendedMat = (frameData1[joint] * (1.0f - blendFactor)) + (frameData2[joint] * blendFactor);
			blendedFrames.emplace_back(blendedMat);
		}
	}

	// Create the blended animation
	MeshAnimation* blendedAnim = new MeshAnimation(jointCount, frameCount, frameRate, blendedFrames);
	return blendedAnim;
}
