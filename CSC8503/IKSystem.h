#pragma once

#include "GameObject.h"
#include "PaintClass.h"
#include <string>

namespace reactphysics3d {
	class RigidBody;
	class Transform;
	class CollisionCallback;
}
namespace NCL::Maths {
	class Vector3;
}

namespace NCL::CSC8503 {
	class IKSystem {
	public:
		IKSystem(float legLengthInput, float stepTimeInput, reactphysics3d::Vector3 bodyOffsetInput, reactphysics3d::Vector3 bodyTargetOffsetInput, GameObject* HostObj, GameWorld* world);
		~IKSystem();

		Vector3 getRootPos() { return rootPos; }
		void setRootPos(Vector3 i) { rootPos = i; }

		Vector3 getJointPos() { return jointPos; }
		void setJointPos(Vector3 i) { jointPos = i; }

		Vector3 getTipPos() { return tipPos; }
		void setTipPos(Vector3 i) { tipPos = i; }

		Vector3 getTargetJointPos() { return targetJointPos; }
		void setTargetJointPos(Vector3 i) { targetJointPos = i; }

		Vector3 getTargetTipPos() { return targetTipPos; }
		void setTargetTipPos(Vector3 i) { targetTipPos = i; }

		Vector3 getIKTargetPos() { return IKTargetPos; }
		void setIKTargetPos(Vector3 i) { IKTargetPos = i; }

		Vector3 getIKIdealTargetPos() { return IKTargetPos; }
		void setIKIdealTargetPos(Vector3 i) { IKTargetPos = i; }

		float getLegLength() { return legLength; }

		void IKSystemUpdate(float dt);

		void IKProcess();

		void setIKLegObj1(GameObject* i) { IKLegObj1 = i; }
		void setIKLegObj2(GameObject* i) { IKLegObj2 = i; }

		GameWorld* getIKHostWorld() { return IKHostWorld; }
		void setIKHostWorld(GameWorld* i) { IKHostWorld = i; }

	protected:
		Vector3 rootPos;

		Vector3 jointPos;

		Vector3 tipPos;

		Vector3 targetJointPos;

		Vector3 targetTipPos;

		Vector3 IKIdealTargetPos;
		Vector3 IKTargetPos;
		Vector3 IKEndTargetPos;

		float legLength;

		bool footOnFloor;
		float stepTime;
		float timer = 0.0f;

		reactphysics3d::Vector3 bodyOffset;
		reactphysics3d::Vector3 bodyTargetOffset;

		GameWorld* IKHostWorld;
		GameObject* IKHostObj = nullptr;
		GameObject* IKLegObj1 = nullptr;
		GameObject* IKLegObj2 = nullptr;

		//void IKLegObjectProcess();
		//Quaternion vectorToRotation(Vector3 input);
	};
}
