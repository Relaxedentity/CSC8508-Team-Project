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

using namespace reactphysics3d;
using namespace NCL::CSC8503;

Projectile::Projectile(GameWorld* world, std::string name) : PaintClass(world,name) {

}

Projectile::~Projectile() {

}

void Projectile::OnCollisionBegin(GameObject* otherObject) {
	if (dynamic_cast<Projectile*>(otherObject)) {
		return;
	}

	/*Sound mod*/
	Vector3 hitposition = collisionPoint - world->GetMainCamera()->GetPosition();//
	Vector3 hitposition2 = collisionPoint - world->GetSecCamera()->GetPosition();//
	ISoundEngine* hit = createIrrKlangDevice();///
	collisionV->HitVoice(hit, hitposition);//
	collisionV->HitVoice(hit, hitposition2);//

	//std::cout << collisionPoint << std::endl;
	//int colourInt = (paintColour == 'r') ? 1 : 2;
	//Vector4 paintCollision = Vector4(collisionPoint, colourInt);

	///Render the first collision object
	if (otherObject)otherObject->GetRenderObject()->PaintSpray(collisionPoint, paintColour);

	//Spherical ray cast with the collisionPoint as the centre of the sphere
	std::vector<GameObject*> collisionObjects;
	std::vector<reactphysics3d::Vector3> vertices= Projectile::CalculateSphereVertices(collisionPoint);
	for (int i = 0; i < vertices.size(); i++)
	{
		reactphysics3d::Vector3 rayOrigin(collisionPoint.x, collisionPoint.y, collisionPoint.z);
		reactphysics3d::Vector3 rayDirection(vertices[i]);
		reactphysics3d::Ray ray = reactphysics3d::Ray(rayOrigin, rayDirection);
		SceneContactPoint* closestCollision = world->Raycast(ray);
		if (closestCollision->isHit) {
			//Debug::DrawLine(collisionPoint, closestCollision->hitPos, Vector4(0, 1, 1, 1), 60);
			
			//Render the objects collided with by spherical rays
			collisionObjects.push_back(closestCollision->object);
		}
	}

	//Removal of duplicate objects
	std::vector<GameObject*>::iterator ite1 = unique(collisionObjects.begin(), collisionObjects.end());
	collisionObjects.erase(ite1, collisionObjects.end());
	for (int i = 0; i < collisionObjects.size(); i++)
	{
		//Debug::DrawLine(collisionPoint, collisionObjects[i]->GetPhysicsObject()->getTransform().getPosition(), Vector4(0, 1, 1, 1), 60);
		collisionObjects[i]->GetRenderObject()->PaintSpray(collisionPoint, paintColour);
	}

	//Release memory
	collisionObjects.clear();
	collisionObjects.shrink_to_fit();


	//world->painted.push_back(paintCollision);

	//std::cout << "projectile paint colour: " << paintColour << "\n";

	world->testPaintNodes(collisionPoint, paintColour);

	reactphysics3d::Transform temp(reactphysics3d::Vector3(0,-50000,0),reactphysics3d::Quaternion::identity());
	GetPhysicsObject()->setTransform(temp);
	GetPhysicsObject()->setType(reactphysics3d::BodyType::STATIC);

}
void Projectile::Update(float dt) {
	time -= dt;
	if (time <= 0) {
		//world->RemoveGameObject(this);
	}
}
/// <summary>
/// Get all the vertices of the sphere
/// </summary>
/// <param name="hitPos"></param>
/// <returns></returns>
std::vector<reactphysics3d::Vector3> Projectile::CalculateSphereVertices(NCL::Vector3 hitPos) {
	const double PI = 3.14159265358979323846;
	double r = 2; // ��뾶  sphere radius
    const int numSlices = 16;    // ˮƽ��Ƭ���� Number of horizontal slices
	const int numStacks = 16;    // ��ֱ��Ƭ���� Number of vertical slices

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