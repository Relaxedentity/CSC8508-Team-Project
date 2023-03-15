#include <reactphysics3d/body/RigidBody.h>
#include "Debug.h"
#include "GameWorld.h"
#include "IKSystem.h"


using namespace reactphysics3d;
using namespace NCL::CSC8503;

IKSystem::IKSystem(float legLengthInput, float stepTimeInput, reactphysics3d::Vector3 bodyOffsetInput, reactphysics3d::Vector3 bodyTargetOffsetInput, GameObject* HostObj, GameWorld* world) {
	IKHostObj = HostObj;
	IKHostWorld = world;
	legLength = legLengthInput;
	stepTime = stepTimeInput;
	footOnFloor = true;
	bodyOffset = bodyOffsetInput;
	bodyTargetOffset = bodyTargetOffsetInput;	
}

IKSystem::~IKSystem() {

}

void IKSystem::IKSystemUpdate(float dt) {
	reactphysics3d::Vector3 newIKIdealTargetPos = IKHostObj->GetPhysicsObject()->getTransform().getPosition() + IKHostObj->GetPhysicsObject()->getTransform().getOrientation() * bodyTargetOffset;
	Debug::DrawPoint(newIKIdealTargetPos, Vector4(1, 0, 0, 1), 0.1f);

	rootPos = IKHostObj->GetPhysicsObject()->getTransform().getPosition() + IKHostObj->GetPhysicsObject()->getTransform().getOrientation() * bodyOffset;
	Debug::DrawPoint(rootPos, Vector4(0, 1, 0, 1), 0.1f);

	Ray r = Ray(rootPos, Vector3(newIKIdealTargetPos) - rootPos);
	RayCollision legCollision;

	Vector3 newIKTargetPos;

	reactphysics3d::Ray ray = reactphysics3d::Ray(reactphysics3d::Vector3(rootPos.x, rootPos.y, rootPos.z), newIKIdealTargetPos);
	SceneContactPoint* legTarget = IKHostWorld->Raycast(ray, IKHostObj);
	if (legTarget->isHit) {
		newIKTargetPos = legTarget->hitPos;
	}

	Debug::DrawPoint(newIKTargetPos, Vector4(1, 1, 0, 1), 0.1f);

	float distToTargetPos = (newIKTargetPos - getTargetTipPos()).Length();

	if (distToTargetPos > 2.0f && footOnFloor == true) {
		footOnFloor = false;
		timer = 0;
		IKTargetPos = targetTipPos;
		IKEndTargetPos = targetTipPos + (newIKTargetPos - targetTipPos) * 1.5;
	}
	if (footOnFloor == false) {
		timer += dt * stepTime;
		//targetTipPos = SLERP(IKTargetPos, newIKTargetPos, stepTime * timer);

		//targetTipPos = IKTargetPos + (newIKTargetPos- IKTargetPos) * timer;
		targetTipPos = IKTargetPos + Vector3(0, 1, 0) + Vector3(0, -1, 0) * (abs(timer - 0.5)) + (IKEndTargetPos - IKTargetPos) * timer;
		if (timer > 1) {
			targetTipPos = IKEndTargetPos;
			footOnFloor = true;
		}
	}

	IKProcess();

	//IKLegObjectProcess();
}

void IKSystem::IKLegObjectProcess() {
	reactphysics3d::Vector3 newPos = reactphysics3d::Vector3(rootPos.x, rootPos.y, rootPos.z) + (reactphysics3d::Vector3(targetJointPos.x, targetJointPos.y, targetJointPos.z) - reactphysics3d::Vector3(rootPos.x, rootPos.y, rootPos.z)) * 0.5;
	reactphysics3d::Transform newTransform = reactphysics3d::Transform(newPos, vectorToRotation(targetJointPos - rootPos));
	IKLegObj1->GetPhysicsObject()->setTransform(newTransform);

	newPos = reactphysics3d::Vector3(targetJointPos.x, targetJointPos.y, targetJointPos.z) + (reactphysics3d::Vector3(targetTipPos.x, targetTipPos.y, targetTipPos.z) - reactphysics3d::Vector3(targetJointPos.x, targetJointPos.y, targetJointPos.z)) * 0.5;
	newTransform = reactphysics3d::Transform(newPos, vectorToRotation(targetTipPos - targetJointPos));
	IKLegObj2->GetPhysicsObject()->setTransform(newTransform);

	//IKLegObj1->GetTransform().SetPosition(rootPos + (targetJointPos - rootPos) * 0.5);
	//IKLegObj1->GetTransform().SetOrientation(vectorToRotation(targetJointPos - rootPos));
	//IKLegObj2->GetTransform().SetPosition(targetJointPos + (targetTipPos - targetJointPos) * 0.5);
	//IKLegObj2->GetTransform().SetOrientation(vectorToRotation(targetTipPos - targetJointPos));
}

reactphysics3d::Quaternion IKSystem::vectorToRotation(NCL::Vector3 input) {
	float pi = 3.14159265358979323846;
	float theta = (atan2(input.z, input.x) * 180) / pi;
	//std::cout << theta << "\n";
	Quaternion YRot = Quaternion(Matrix4::Rotation(theta, Vector3(0, 1, 0)));
	Quaternion antiYRot = Quaternion(Matrix4::Rotation(-theta, Vector3(0, 1, 0)));
	Vector3 AtoC = YRot * input;

	theta = (atan2(AtoC.x, AtoC.y) * 180) / pi;
	Quaternion XRot = Quaternion(Matrix4::Rotation(-theta, Vector3(0, 0, 1)));
	XRot = antiYRot * XRot;
	return reactphysics3d::Quaternion(XRot.x, XRot.y, XRot.z, XRot.w);
}

void IKSystem::IKProcess() {
	float theta = (atan2((targetTipPos.z - rootPos.z), (targetTipPos.x - rootPos.x)) * 180) / PI_RP3D;
	//std::cout << theta << "\n";
	Quaternion YRot = Quaternion(Matrix4::Rotation(theta, Vector3(0, 1, 0)));
	Quaternion antiYRot = Quaternion(Matrix4::Rotation(-theta, Vector3(0, 1, 0)));
	Vector3 AtoC = YRot * (targetTipPos - rootPos);

	//Debug::DrawLine(rootPos, rootPos + AtoC, Vector4(0, 0, 1, 1));

	float AtoCLength = (targetTipPos - rootPos).Length();
	float aAngle = (acos(pow(AtoCLength, 2) / (2 * AtoCLength * legLength)) * 180) / PI_RP3D;
	Quaternion ZRot = Quaternion(Matrix4::Rotation(aAngle, Vector3(0, 0, 1)));
	Vector3 jointTempPos = ZRot * (AtoC.Normalised() * legLength);
	targetJointPos = rootPos + (antiYRot * jointTempPos);
	Debug::DrawLine(rootPos, targetJointPos, Vector4(0, 0, 1, 1));
	Debug::DrawLine(targetJointPos, targetTipPos, Vector4(0, 0, 1, 1));
}