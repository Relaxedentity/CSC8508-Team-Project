#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/reactphysics3d.h>
#include "GameObject.h"
#include "CollisionDetection.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Debug.h"
#include "GameWorld.h"
#include "Projectile.h"
#include "Vector3.h"
#include <iostream>
#include "TutorialGame.h"
#include <iterator>
#include "Sound.h"
#include "Gamelock.h"

using namespace reactphysics3d;
using namespace NCL::CSC8503;

Projectile::Projectile(TutorialGame* t, GameWorld* world, float max_time, std::string name) : PaintClass(world,name) {
	this->max_time = max_time;
	time = 0;
	particleTime = 0;
	tgame = t;
}

Projectile::~Projectile() {
	delete collisionV;
}

void Projectile::OnCollisionBegin(GameObject* otherObject) {
	if (dynamic_cast<Projectile*>(otherObject)) {
		return;
	}

	burstPos = GetPhysicsObject()->getTransform().getPosition();

	/*Sound mod*/
	Vector3 hitposition = collisionPoint - world->GetMainCamera()->GetPosition();//
	Vector3 hitposition2 = collisionPoint - world->GetSecCamera()->GetPosition();//
	collisionV->HitVoice(GameLock::hitsound, hitposition);//
	collisionV->HitVoice(GameLock::hitsound, hitposition2);//

	//std::cout << collisionPoint << std::endl;
	//int colourInt = (paintColour == 'r') ? 1 : 2;
	//Vector4 paintCollision = Vector4(collisionPoint, colourInt);

	///Render the first collision object
	//if (otherObject)otherObject->GetRenderObject()->PaintSpray(collisionPoint, paintColour);

	world->paintSphereTest(this, collisionPoint, paintColour);
	if (otherObject == world->GetPlayerCoop() && world->GetPlayerCoopHealth() >= 0.0f){
		world->SetPlayerCoopHealth(world->GetPlayerCoopHealth() - 0.05f);
	}
	else if (otherObject == world->GetPlayer() && world->GetPlayerHealth() >= 0.0f) {
		world->SetPlayerHealth(world->GetPlayerHealth() - 0.05f); 
	}
	

	//world->painted.push_back(paintCollision);

	//std::cout << "projectile paint colour: " << paintColour << "\n";

	//world->testPaintNodes(collisionPoint, paintColour);

	reactphysics3d::Transform temp(reactphysics3d::Vector3(0,-100,0),reactphysics3d::Quaternion::identity());
	GetPhysicsObject()->setType(reactphysics3d::BodyType::STATIC);
	GetPhysicsObject()->setTransform(temp);


	reactphysics3d::Vector3 tempCollision = reactphysics3d::Vector3(collisionPoint.x, collisionPoint.y, collisionPoint.z);
	tempParticles.push_back( tgame->AddEmitterToWorld(tempCollision, reactphysics3d::Quaternion::identity(),paintColour));
	particleTime = 0.3f;

}
void Projectile::Update(float dt) {
	time -= dt;
	particleTime -= dt;
	if (time <= 0) {
		//world->RemovePaintBall();
		//world->RemoveGameObject(this);
		//tempParticles = NULL;
		reactphysics3d::Transform temp(reactphysics3d::Vector3(0, -100, 0), reactphysics3d::Quaternion::identity());
		GetPhysicsObject()->setType(reactphysics3d::BodyType::STATIC);
		GetPhysicsObject()->setTransform(temp);
	}
	if (particleTime <= 0) {
		for (int i = 0; i < tempParticles.size(); i++)
		{
			world->RemoveGameObject(tempParticles[i]);
			tempParticles[i] = NULL;
			delete tempParticles[i];
		}
		tempParticles.clear();
		tempParticles.shrink_to_fit();
	}

	if (tempParticles.size()) {
		for (int i = 0; i < tempParticles.size(); i++)
		{
			if (tempParticles[i]) {
				reactphysics3d::Transform transform = tempParticles[i]->GetPhysicsObject()->getTransform();
				reactphysics3d::Vector3 collisionPos = reactphysics3d::Vector3(collisionPoint.x, collisionPoint.y, collisionPoint.z);
				reactphysics3d::Vector3 ray = burstPos - collisionPos;
			/*	Debug::DrawPoint(collisionPoint,Vector4(0, 1, 1, 1),60);
				Debug::DrawPoint(burstPos, Vector4(1, 0, 1, 1), 60);
				Debug::DrawLine(collisionPos, burstPos, Vector4(1, 0, 0, 1), 60);*/
				tempParticles[i]->GetPhysicsObject()->setTransform(reactphysics3d::Transform(transform.getPosition() + ray * 10 * dt, transform.getOrientation()));
				reactphysics3d::Vector3 vec = tempParticles[i]->GetPhysicsObject()->getTransform().getPosition();
				/*std::cout << "位置是Location is:" << Vector3(vec.x, vec.y, vec.z) << std::endl;*/
			}
		}	
	}
}
/// <summary>
/// Get all the vertices of the sphere
/// </summary>
/// <param name="hitPos"></param>
/// <returns></returns>
std::vector<reactphysics3d::Vector3> Projectile::CalculateSphereVertices(NCL::Vector3 hitPos) {
	const double PI = 3.14159265358979323846;
	double r = 2; // 球半径  sphere radius
    const int numSlices = 16;    // 水平切片数量 Number of horizontal slices
	const int numStacks = 16;    // 垂直切片数量 Number of vertical slices

	std::vector<reactphysics3d::Vector3> vertices;

	for (int j = 0; j <= numStacks; j++) {
		float theta = j * PI / numStacks;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (int i = 0; i <= numSlices; i++) {
			float phi = i * 2 * PI / numSlices;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float xCoord = hitPos.x + r * sinTheta * cosPhi;
			float yCoord = hitPos.y + r * sinTheta * sinPhi;
			float zCoord = hitPos.z + r * cosTheta;

			vertices.push_back({ xCoord, yCoord, zCoord });
			//std::cout << xCoord << yCoord << zCoord << std::endl;
		}
	}
	return vertices;
}

void Projectile::Reset() {
	time = max_time;
}