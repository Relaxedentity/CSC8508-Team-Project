#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "NetworkObject.h"
#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "BTreeObject.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Assets.h"



using namespace NCL;
using namespace CSC8503;
vector <Vector3 > nodes;
vector <Vector3 > nodes2;
bool onFloor;
TutorialGame::TutorialGame()	{
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);
	testStateObject = nullptr;
	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;
	initSplitScreen = false;
	coopMode = true;

	InitialiseAssets();

}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	charMesh	= renderer->LoadMesh("goat.msh");
	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	gooseMesh = renderer->LoadMesh("goose.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitCameraSec();
	InitWorld();

}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;
	delete capsuleMesh;
	delete gooseMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {

	//LockedObjectMovement();
	MovePlayer(player);

	//Debug::Print(std::to_string(player->getScore()), Vector2(5, 95), Debug::RED);
	//Debug::Print(std::to_string(world->GetObjectCount()), Vector2(95, 5), Debug::RED);

	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
		world->GetSecCamera()->UpdateCamera(dt);
	}

	if (lockFirstObject) {
		Vector3 objPos = lockFirstObject->GetTransform().GetPosition();
		Vector3 camPos = (objPos + lockFirstObject->GetTransform().GetOrientation()* lockedOffset);
		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0,1,0));
		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	if (lockSecObject) {
		//std::cout << lockSecObject << std::endl;
		Vector3 objPos2 = lockSecObject->GetTransform().GetPosition();
		Vector3 camPos2 = objPos2 + lockSecObject->GetTransform().GetOrientation() * lockedOffset;

		Matrix4 temp2 = Matrix4::BuildViewMatrix(camPos2, objPos2, Vector3(0, 1, 0));
		Matrix4 modelMat2 = temp2.Inverse();

		Quaternion q2(modelMat2);
		Vector3 angles2 = q2.ToEuler(); //nearly there now!

		world->GetSecCamera()->SetPosition(camPos2);
		world->GetSecCamera()->SetPitch(angles2.x);
		world->GetSecCamera()->SetYaw(angles2.y);
	}

	UpdateKeys();

	/*RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				Debug::DrawLine(selectionObject->GetTransform().GetPosition(), objClosest->GetTransform().GetPosition(), Vector4(1, 0, 0, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}*/

	//if (testStateObject) {
	//	testStateObject->Update(dt);
	//	for (int i = 1; i < nodes.size(); ++i) {
	//		Vector3 a = nodes[i - 1];
	//		Vector3 b = nodes[i];

	//		//Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	//	}
	//}
	//if (goose) {
	//	goose->Update(dt);
	//	for (int i = 1; i < nodes2.size(); ++i) {
	//		Vector3 a = nodes2[i - 1];
	//		Vector3 b = nodes2[i];

	//		//Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	//	}
	//}


	//SelectObject();
	//MoveSelectedObject();
	world->UpdateWorld(dt);


	renderer->Update(dt);

	if (initSplitScreen && coopMode) {
		renderer->RenderSplitScreens();
		MovePlayerTwo(*playerCoop);
	}else
		renderer->Render();
	
	physics->Update(dt);
	
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateKeys() {
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
	//	InitWorld(); //We can reset the simulation at any time with F1
	//	selectionObject = nullptr;
	//}

	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
	//	InitCamera(); //F2 will reset the camera to a specific default place
	//}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::V) ) {
		
		initSplitScreen ? initSplitScreen = false : initSplitScreen = true;
	}


	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
	//	world->ShuffleConstraints(true);
	//}
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
	//	world->ShuffleConstraints(false);
	//}

	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
	//	world->ShuffleObjects(true);
	//}
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
	//	world->ShuffleObjects(false);
	//}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::H)) {
		std::fstream my_file;
		my_file.open(Assets::DATADIR + "Highscore.txt", std::ios::in);
		if (my_file.fail()) {
			std::cout << "No such file exists Highscore.txt";
		}

		std::string output_int;
		int x = 0;
		while (std::getline(my_file, output_int, ',')) {
			x = std::stoi(output_int);
			my_file.close();
		}
		std::cout << "Highest Score is "<< x <<std::endl;
	}


	/*if (lockFirstObject && lockSecObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}*/
}

void TutorialGame::MovePlayer(GameObject* player) {
	
	RayCollision floorCollision;
	Ray r = Ray(player->GetTransform().GetPosition() + Vector3(0, -1, 0), Vector3(0, -1, 0));
	Debug::DrawLine(player->GetTransform().GetPosition() + Vector3(0, -1, 0), player->GetTransform().GetPosition() + Vector3(0, -2, 0), Vector4(0, 1, 1, 1));

	if (world->Raycast(r, floorCollision, true, selectionObject)) {
		float d = floorCollision.rayDistance;
		if (d < 2) {
			onFloor = true;
		}
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		player->GetPhysicsObject()->AddForce(player->GetTransform().GetOrientation() * Vector3(0, 0, -20));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		player->GetPhysicsObject()->AddTorque(Vector3(0, -2, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE) && onFloor == true) {
		player->GetPhysicsObject()->AddForce(Vector3(0, 1000, 0));
		onFloor = false;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		player->GetPhysicsObject()->AddTorque(Vector3(0, 2, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		player->GetPhysicsObject()->AddForce(player->GetTransform().GetOrientation() * Vector3(0, 0, 20));
	}
}

void NCL::CSC8503::TutorialGame::MovePlayerTwo(GameObject& player)
{
	RayCollision floorCollision;
	Ray r = Ray(player.GetTransform().GetPosition() + Vector3(0, -1, 0), Vector3(0, -1, 0));
	Debug::DrawLine(player.GetTransform().GetPosition() + Vector3(0, -1, 0), player.GetTransform().GetPosition() + Vector3(0, -2, 0), Vector4(0, 1, 1, 1));

	if (world->Raycast(r, floorCollision, true, selectionObject)) {
		float d = floorCollision.rayDistance;
		if (d < 2) {
			onFloor = true;
		}
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		player.GetPhysicsObject()->AddForce(player.GetTransform().GetOrientation() * Vector3(0, 0, -20));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		player.GetPhysicsObject()->AddTorque(Vector3(0, -2, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::V) && onFloor == true) {
		player.GetPhysicsObject()->AddForce(Vector3(0, 1000, 0));
		onFloor = false;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		player.GetPhysicsObject()->AddTorque(Vector3(0, 2, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		player.GetPhysicsObject()->AddForce(player.GetTransform().GetOrientation() * Vector3(0, 0, 20));
	}

}


void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
	//	lockedObject->GetPhysicsObject()->AddForce(lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, -7));
	//}
	//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
	//	lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, 2, 0));
	//}
	//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
	//	lockedObject->GetPhysicsObject()->AddForce(Vector3(0, 50, 0));
	//}
	//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
	//	lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, -2, 0));
	//}
	//
	//if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
	//	lockedObject->GetPhysicsObject()->AddForce(lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 7));
	//}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		lockFirstObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	//lockFirstObject = nullptr;
}

void NCL::CSC8503::TutorialGame::InitCameraSec()
{
	world->GetSecCamera()->SetNearPlane(0.1f);
	world->GetSecCamera()->SetFarPlane(500.0f);
	world->GetSecCamera()->SetPitch(-15.0f);
	world->GetSecCamera()->SetYaw(315.0f);
	world->GetSecCamera()->SetPosition(Vector3(10, 30, 60));
	//lockedObject = nullptr;s
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();
	BridgeConstraintTest(Vector3(0,-20,-310));
	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	InitGameExamples();
	
	Vector3 startPos(70, 0, -10);
	TestPathfinding(startPos);

	testStateObject = AddStateObjectToWorld(nodes[0],nodes);
	
	button = AddButtonToWorld(Vector3(0, -18, 0));

	buildGameworld();

	AddHedgeMazeToWorld();
	door = AddGWBlocksToWorld(Vector3(50,-13,50), Vector3(5, 5, 5));
	door->GetRenderObject()->SetColour(Vector4(0,0,1,1));
	door->GetPhysicsObject()->SetInverseMass(0.5f);
	door->SetTag(4);
	button->SetAssociated(door);
	TestHedgefinding(Vector3(0, 5, 0));
	//goose = AddGooseToWorld(nodes2[0], nodes2);
	InitDefaultFloor();
	
}

void TutorialGame::buildGameworld() {
	srand(time(0));
	int i = 0;
	worldGrid = new NavigationGrid("TestGrid2.txt");
	GridNode* nodes = worldGrid->GetAllNodes();
	int gridwidth = worldGrid->GetGridWidth();
	int gridheight = worldGrid->getGridHeight();
	for (int y = 0; y < gridheight; y++) {
		for (int x = 0; x < gridwidth; x++) {
			GridNode& n = nodes[(gridwidth * y) + x];
			Vector3 Nposition = n.position;
			GameObject* object;
			switch (n.type) {
			case '.':
				break;

			case 'x':
				object = AddGWBlocksToWorld(Vector3(Nposition.x-200.0f, Nposition.y - 13.0f, Nposition.z - 50.0f), Vector3(5, 5, 5));
				break;

			case 'b':
				object = AddBreakableToWorld(Vector3(Nposition.x - 200.0f, Nposition.y - 13.0f, Nposition.z - 50.0f), 1.0f);
				i++;
				world->SetObjectCount(i);
				break;
			}
		}
	}
}


void TutorialGame::AddHedgeMazeToWorld() {
	srand(time(0));
	worldGrid = new NavigationGrid("TestGrid3.txt");
	GridNode* nodes = worldGrid->GetAllNodes();
	int gridwidth = worldGrid->GetGridWidth();
	int gridheight = worldGrid->getGridHeight();
	for (int y = 0; y < gridheight; y++) {
		for (int x = 0; x < gridwidth; x++) {
			GridNode& n = nodes[(gridwidth * y) + x];
			Vector3 Nposition = n.position;
			GameObject* object;
			switch (n.type) {
			case '.':
				break;

			case 'x':
				object = AddGWBlocksToWorld(Vector3(Nposition.x-45.0f , Nposition.y-13.0f , Nposition.z- 450.0f ), Vector3(5, 5, 5));
				break;

			case 'b':
				object = AddBreakableToWorld(Vector3(Nposition.x - 45.0f, Nposition.y- 13.0f, Nposition.z - 450.0f), 1.0f);
				break;
			}
		}
	}
}
/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position,Vector3 size) {
	GameObject* floor = new GameObject();

	AABBVolume* volume = new AABBVolume(size);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(size * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));
	sphere->GetPhysicsObject()->setRestitution(0.22f);

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}
GameObject* TutorialGame::AddBreakableToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();
	sphere->SetTag(2);
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));
	sphere->GetPhysicsObject()->setRestitution(0.22f);
	sphere->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}
GameObject* TutorialGame::AddGWBlocksToWorld(const Vector3& position, Vector3 dimensions) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}
GameObject* TutorialGame::AddButtonToWorld(const Vector3& position, float inverseMass) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(2, 0.1f, 2);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);
	floor->SetTag(3);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position, int netID, int worldID) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	character->SetTag(1);
	character->setScore(0);
	CapsuleVolume* volume = new CapsuleVolume(2.5f, 1.5f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();
	character->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

	world->AddGameObject(character);
	character->SetWorldID(worldID);
	NetworkObject* n = new NetworkObject(*character, netID);

	return character;
}

GameObject* TutorialGame::AddPlayerForCoop(const Vector3& position) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	character->SetTag(1);
	character->setScore(0);
	CapsuleVolume* volume = new CapsuleVolume(2.5f, 1.5f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();
	character->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

	world->AddGameObject(character);
	//character->SetWorldID(2);
	//NetworkObject* n = new NetworkObject(*character, 2);
	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(2.0f, 0.5f);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}
StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position, vector <Vector3 > testNodes) {
	StateGameObject* apple = new StateGameObject(testNodes);
	
	AABBVolume* volume = new AABBVolume(Vector3(3,5,3));
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(5, 5, 5))
		.SetPosition(position);
	apple->setTarget(player);
	apple->SetTag(5);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), enemyMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(0.5f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}
BTreeObject* TutorialGame::AddGooseToWorld(const Vector3& position, vector <Vector3 > testNodes) {
	BTreeObject* apple = new BTreeObject(testNodes);

	AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);
	apple->setTarget1(player);
	
	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), gooseMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));
	apple->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
	apple->GetPhysicsObject()->SetInverseMass(0.5f);
	apple->GetPhysicsObject()->InitSphereInertia();
	NetworkObject* n = new NetworkObject(*apple, 3);
	world->AddGameObject(apple);

	return apple;
}
void TutorialGame::TestPathfinding(Vector3 position) {
	NavigationGrid grid("TestGrid2.txt");
	NavigationPath outPath;

	Vector3 startPos(0, 0, 0);
	Vector3 endPos(50, 0, 80);

	
	bool found = grid.FindOffsetPath(position, endPos, outPath, Vector3(-50, 0, -50));

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		pos.x = pos.x - 200.0f;
		pos.y = pos.y - 10.0f;
		pos.z = pos.z - 50.0f;
		nodes.push_back(pos);
	}
}
void TutorialGame::TestHedgefinding(Vector3 position) {
	NavigationGrid grid("TestGrid3.txt");
	NavigationPath outPath;

	Vector3 startPos(20, 0, -30);
	Vector3 endPos(5, 0, 40);

	bool found = grid.FindOffsetPath(startPos, endPos, outPath,Vector3(-50,0,-50));
	//bool found = grid.FindPath(startPos, endPos, outPath);
	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		pos.x = pos.x - 45.0f;
		pos.y = pos.y - 16.0f;
		pos.z = pos.z - 450.0f;
		nodes2.push_back(pos);
	}
}

void TutorialGame::InitDefaultFloor() {
	Vector3 floorSize = Vector3(200, 2, 150);
	AddFloorToWorld(Vector3(0, -20, 0),floorSize);
	Vector3 hedgeSize = Vector3(100, 2, 75);
	AddFloorToWorld(Vector3( 0, -20, -400),hedgeSize);
}

void TutorialGame::InitGameExamples() {
	player = AddPlayerToWorld(Vector3(5, 5, -330), 1,1);

	playerCoop = AddPlayerForCoop(Vector3(-10, 5, -200));

	LockCameraToObject(player);
	LockCameraToObject2(playerCoop);

	patrol = AddEnemyToWorld(Vector3(-20, 5, 20));
	AddBonusToWorld(Vector3(10, 5, 0));
	world->SetPlayer(player);
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	Vector3 floorSize = Vector3(200, 2, 150);
	AddFloorToWorld(Vector3(0, -2, 0),floorSize);
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::BridgeConstraintTest(Vector3 pos) {
	Vector3 cubeSize = Vector3(5, 5, 5);
	float invCubeMass = 5; //how heavy the middle pieces are
	int numLinks = 7;
	float maxDistance = 20; // constraint distance
	float cubeDistance = 17; // distance between links

	Vector3 startPos = pos;

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0)
		, cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3(0, 0, (numLinks + 2)
		* cubeDistance), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) *
			cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint1 = new PositionConstraint(previous,
			block, maxDistance);
		OrientationConstraint* constraint2 = new OrientationConstraint(previous,
			block);
		world->AddConstraint(constraint1);
		world->AddConstraint(constraint2);
		previous = block;
	}
	PositionConstraint* constraintD = new PositionConstraint(previous,
		end, maxDistance);
	world->AddConstraint(constraintD);

	OrientationConstraint* constraintO = new OrientationConstraint(previous,
		end);
	world->AddConstraint(constraintO);
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(true);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		//Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockFirstObject == selectionObject) {
					lockFirstObject = nullptr;
					//lockSecObject = nullptr;
					//lockSecObject = nullptr;
				}
				else {
					lockFirstObject = selectionObject;
					//lockSecObject = selectionObjectSec;
				}
			}
		}
	}
	else {
		//Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	//Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void patrolMovement() {

 }

