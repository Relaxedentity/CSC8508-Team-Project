#include "OrientationConstraint.h"
#include "GameObject.h"
#include "PhysicsObject.h"
using namespace NCL;
using namespace Maths;
using namespace CSC8503;

OrientationConstraint::OrientationConstraint(GameObject* a, GameObject* b)
{
	objectA = a;
	objectB = b;
}

OrientationConstraint::~OrientationConstraint()
{

}

void OrientationConstraint::UpdateConstraint(float dt) {

	/*Vector3 a = objectA->GetTransform().GetOrientation() * Vector3(1, 0, 0);
	Vector3 b = objectB->GetTransform().GetOrientation()* Vector3(1, 0, 0);

	float offset = Vector3::Dot(a, b);
	Vector3 offsetDir = Vector3::Cross(a, b);

	if (abs(offset) > 0.0f) {
		

		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		Vector3 relativeAngular = physA->GetAngularVelocity() -
			physB->GetAngularVelocity();

		float constraintMass = physA->GetInverseMass() +
			physB->GetInverseMass();

		if (constraintMass > 0.0f) {
			//how much of their relative force is affecting the constraint
			float velocityDot = Vector3::Dot(relativeAngular, offsetDir);

			float biasFactor = 0.01f;
			float bias = -(biasFactor / dt) * offset;

			float lambda = -(velocityDot + bias) / constraintMass;

			Vector3 aImpulse = offsetDir * lambda;
			Vector3 bImpulse = -offsetDir * lambda;

			physA->ApplyAngularImpulse(aImpulse);// multiplied by mass here
			physB->ApplyAngularImpulse(bImpulse);// multiplied by mass here
		}
	}
	a= objectA->GetTransform().GetOrientation() * Vector3(0, 1, 0);
	b = objectB->GetTransform().GetOrientation() * Vector3(0, 1, 0);

	offset = Vector3::Dot(a, b);
	offsetDir = Vector3::Cross(a, b);

	if (abs(offset) > 0.0f) {


		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		Vector3 relativeAngular = physA->GetAngularVelocity() -
			physB->GetAngularVelocity();

		float constraintMass = physA->GetInverseMass() +
			physB->GetInverseMass();

		if (constraintMass > 0.0f) {
			//how much of their relative force is affecting the constraint
			float velocityDot = Vector3::Dot(relativeAngular, offsetDir);

			float biasFactor = 0.01f;
			float bias = -(biasFactor / dt) * offset;

			float lambda = -(velocityDot + bias) / constraintMass;

			Vector3 aImpulse = offsetDir * lambda;
			Vector3 bImpulse = -offsetDir * lambda;

			physA->ApplyAngularImpulse(aImpulse);// multiplied by mass here
			physB->ApplyAngularImpulse(bImpulse);// multiplied by mass here
		}
	}
	a = objectA->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	b = objectB->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	offset = Vector3::Dot(a, b);
	offsetDir = Vector3::Cross(a, b);

	if (abs(offset) > 0.0f) {


		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		Vector3 relativeAngular = physA->GetAngularVelocity() -
			physB->GetAngularVelocity();

		float constraintMass = physA->GetInverseMass() +
			physB->GetInverseMass();

		if (constraintMass > 0.0f) {
			//how much of their relative force is affecting the constraint
			float velocityDot = Vector3::Dot(relativeAngular, offsetDir);

			float biasFactor = 0.01f;
			float bias = -(biasFactor / dt) * offset;

			float lambda = -(velocityDot + bias) / constraintMass;

			Vector3 aImpulse = offsetDir * lambda;
			Vector3 bImpulse = -offsetDir * lambda;

			physA->ApplyAngularImpulse(aImpulse);// multiplied by mass here
			physB->ApplyAngularImpulse(bImpulse);// multiplied by mass here
		}
	}*/
}