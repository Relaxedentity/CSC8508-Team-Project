#include <reactphysics3d/reactphysics3d.h>
#include "TutorialGame.h"
#include "GameWorld.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "NetworkObject.h"
#include "StateGameObject.h"
#include "BTreeObject.h"
#include "Debug.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Assets.h"
#include "../OpenGLRendering/OGLRenderer.h"
#include <OGLRenderer.cpp>
#include "Maths.h"
#include "Projectile.h"

using namespace NCL;
using namespace CSC8503;
vector <NCL::Maths::Vector3> nodes;
vector <NCL::Maths::Vector3> nodes2;
TutorialGame::TutorialGame()	{
	reactphysics3d::PhysicsWorld::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 15; // Default is 10, we can discuss reducing to default
	settings.defaultPositionSolverNbIterations = 8; // Default is 5, we can discuss reducing to default
	settings.isSleepingEnabled = false;
	settings.gravity = reactphysics3d::Vector3(0, -9.81, 0);
	physicsWorld = physics.createPhysicsWorld(settings);
	world		= new GameWorld(physicsWorld);
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	testStateObject = nullptr;
	forceMagnitude	= 10.0f;
	useGravity		= true;
	inSelectionMode = false;
	freeCamera		= false;
	world->SetPlayerHealth(1.0f);
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
	charShader = renderer->LoadShader("charVert.vert", "charFrag.frag");

	//Rebellion assets
	//testMesh = renderer->LoadMesh("Rig_Maximilian.msh");

	corridorTexture			= renderer->LoadTexture("corridor_wall_c.tga");
	corridorStraightMesh	= renderer->LoadMesh("corridor_Wall_Straight_Mid_end_L.msh");
	corridorCornerRightSideMesh	= renderer->LoadMesh("Corridor_Wall_Corner_Out_L.msh");
	corridorCornerLeftSideMesh	= renderer->LoadMesh("Corridor_Wall_Corner_Out_R.msh");
	
	chairTex	= renderer->LoadTexture("InSanct_Max_Chairs_Colour.tga");
	chairMesh	= renderer->LoadMesh("SanctumChair.msh");

	testMesh = renderer->LoadMesh("Rig_Maximilian.msh");

	timeLimit = 300;

	InitCamera();
	InitWorld();

	GameObjectListener* listener = new GameObjectListener(world);
	world->SetCollisionListener(listener);
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


	delete chairTex;
	delete chairMesh;

	delete corridorTexture;
	delete corridorStraightMesh;
	delete corridorCornerRightSideMesh;
	delete corridorCornerLeftSideMesh;

	delete testMesh;

	delete renderer;
	delete world;
	physics.destroyPhysicsWorld(physicsWorld);
}

void TutorialGame::UpdateGame(float dt) {
	Debug::DrawAxisLines(Matrix4());


	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E) && freeCamera) {
		inSelectionMode = !inSelectionMode;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		freeCamera = !freeCamera;
		if (!freeCamera) inSelectionMode = false;
	}

	if (freeCamera) {
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(true);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
		world->GetMainCamera()->UpdateCamera(dt);
	}
	else {
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(true);
		if (lockedObject == player) {
			MovePlayer(player, dt);
		}
	}



	world->SetPlayerHealth(health);
	timeLimit -= dt;


	Debug::Print(std::to_string((int)timeLimit), Vector2(47, 4), Debug::WHITE);
	
	UpdateKeys();

	if (useGravity) {
		//Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		//Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		reactphysics3d::Vector3 rayDir = selectionObject->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, -1);
		reactphysics3d::Vector3 rayPos = selectionObject->GetPhysicsObject()->getTransform().getPosition();

		reactphysics3d::Ray ray = reactphysics3d::Ray(rayPos, rayPos + rayDir * 1000);
		SceneContactPoint* closestCollision = world->Raycast(ray);
		if (closestCollision->isHit) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				//Debug::DrawLine(Vector3(rayPos), Vector3(rayPos + rayDir * 1000), Vector4(1, 0, 0, 1));
			}
			objClosest = (GameObject*)closestCollision->object;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	if (testStateObject) {
		testStateObject->Update(dt);
		for (int i = 1; i < nodes.size(); ++i) {
			Vector3 a = nodes[i - 1];
			Vector3 b = nodes[i];

			//Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
		}
	}
	if (goose) {
		goose->Update(dt);
		for (int i = 1; i < nodes2.size(); ++i) {
			Vector3 a = nodes2[i - 1];
			Vector3 b = nodes2[i];

			//Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
		}

		reactphysics3d::Transform transform = goose->GetPhysicsObject()->getTransform();
		transform.setPosition(reactphysics3d::Vector3(0, 0, 1));
	}

	SelectObject();
	MoveSelectedObject();
	world->OperateOnContents([&](GameObject* o) {o->Update(dt); });
	world->UpdateWorld(dt);
	physicsWorld->update(dt);
	renderer->Update(dt);

	renderer->Render();
	
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physicsWorld->setIsGravityEnabled(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	/* if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}*/
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

	if (lockedObject) {
		//LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}
void TutorialGame::MovePlayer(GameObject* player, float dt) {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
		thirdPerson = !thirdPerson;
	}

	world->GetMainCamera()->ThirdPersonUpdateRot();

	reactphysics3d::Transform playerTransform = player->GetPhysicsObject()->getTransform();
	Vector3 objPos = Vector3(playerTransform.getPosition());

	Quaternion goatStartRotation;
	Quaternion goatRealRotation;

	Vector3 camPos;
	if (!thirdPerson) {
		camPos = orbitCameraProcess(objPos);
		cameraInterpolation(camPos, dt);
	}
	else {
		camPos = thirdPersonCameraProcess(objPos);
		cameraInterpolation(camPos, dt);
		camPos = world->GetMainCamera()->GetPosition();
	}

	Quaternion Yaw = Quaternion(world->GetMainCamera()->GetRotationYaw());

	Vector3 startVelocity = lockedObject->GetPhysicsObject()->getLinearVelocity();
	Vector3 endVelocity = Vector3(0, 0, 0);

	bool onFloor = false;
	reactphysics3d::Ray ray = reactphysics3d::Ray(playerTransform.getPosition(), playerTransform.getPosition() + reactphysics3d::Vector3(0, -5, 0));
	SceneContactPoint* ground = world->Raycast(ray, player);
	if (ground->isHit) {
		onFloor = true;
	}

	bool directionInput = false;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		Vector3 trajectory = onFloor ? Yaw * Vector3(0, 0, -15) : Yaw * Vector3(0, 0, -7);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(0, 0, -1);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		Vector3 trajectory = onFloor ? Yaw * Vector3(0, 0, 15) : Yaw * Vector3(0, 0, 7);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(0, 0, 1);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		Vector3 trajectory = onFloor ? Yaw * Vector3(-15, 0, 0) : Yaw * Vector3(-7, 0, 0);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(-1, 0, 0);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		Vector3 trajectory = onFloor ? Yaw * Vector3(15, 0, 0) : Yaw * Vector3(7, 0, 0);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(1, 0, 0);
		directionInput = true;
	}
	if (!directionInput && onFloor) {
		float scalar = (1 - dt);
		player->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(startVelocity.x * scalar, startVelocity.y, startVelocity.z * scalar));
	}

	if (directionInput && (endVelocity.Normalised() - Vector3(startVelocity).Normalised()).Length() > 1.25 && onFloor) {
		endVelocity.Normalise();
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(endVelocity.x, endVelocity.y, endVelocity.z) * 10);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && onFloor) {
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(0, 1000, 0));
	}

	if (!thirdPerson) {
		Vector3 currentVelocity = Vector3(lockedObject->GetPhysicsObject()->getLinearVelocity());
		float theta = atan2(currentVelocity.z, currentVelocity.x) * (180 / PI);
		Quaternion goatTargetRotation = Quaternion(Matrix4::Rotation(-theta - 90, Vector3(0, 1, 0)));
		goatStartRotation = Quaternion(player->GetPhysicsObject()->getTransform().getOrientation());
		goatRealRotation = Quaternion::Lerp(goatStartRotation, goatTargetRotation, 0.5f);
	}
	else {
		Quaternion goatTargetRotation = thirdPersonRotationCalc(world, player, world->GetMainCamera(), camPos);
		goatStartRotation = Quaternion(player->GetPhysicsObject()->getTransform().getOrientation());
		goatRealRotation = Quaternion::Lerp(goatStartRotation, goatTargetRotation, 0.25f);
	}

	reactphysics3d::Transform newTransform = reactphysics3d::Transform(reactphysics3d::Vector3(objPos.x, objPos.y, objPos.z), reactphysics3d::Quaternion(goatRealRotation.x, goatRealRotation.y, goatRealRotation.z, goatRealRotation.w));
	
	player->GetPhysicsObject()->setTransform(newTransform);

	//reactphysics3d::Quaternion tempQuart = reactphysics3d::Quaternion(goatStartRotation.x, goatStartRotation.y, goatStartRotation.z, goatStartRotation.w);
	//tempQuart.inverse();
	//goatRealRotation = Quaternion(tempQuart) * goatRealRotation;
	//Vector3 torqueVector = goatRealRotation.ToEuler();
	//
	//player->GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(torqueVector.x*15, torqueVector.y * 15, torqueVector.z * 15));

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F)) {
		Projectile* projectile = AddSphereToWorld(player->GetPhysicsObject()->getTransform().getPosition() + player->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, -3), reactphysics3d::Quaternion(0, 0, 0, 1), 0.5);
		
		Quaternion Pitch = Quaternion(world->GetMainCamera()->GetRotationPitch());
		reactphysics3d::Quaternion reactPitch = reactphysics3d::Quaternion(Pitch.x, Pitch.y, Pitch.z, Pitch.w);
		
		projectile->GetPhysicsObject()->applyWorldForceAtCenterOfMass(player->GetPhysicsObject()->getTransform().getOrientation() * reactPitch * reactphysics3d::Vector3(0, 0, -250));
	}



	// splines, curves, improve interpolation, TCB curves
}

Quaternion TutorialGame::thirdPersonRotationCalc(GameWorld* world, GameObject* object, Camera* cam, Vector3 camPos) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();
	Vector3 nearPos = Vector3(screenSize.x / 2,
		screenSize.y / 2,
		-0.99999f
	);
	Vector3 farPos = Vector3(screenSize.x / 2,
		screenSize.y / 2,
		0.99999f
	);

	Vector3 a = CollisionDetection::Unproject(nearPos, *cam);
	Vector3 b = CollisionDetection::Unproject(farPos, *cam);
	Vector3 c = b - a;
	c.Normalise();

	Quaternion goatStartRotation;
	reactphysics3d::Ray ray = reactphysics3d::Ray(reactphysics3d::Vector3(camPos.x, camPos.y, camPos.z), reactphysics3d::Vector3(camPos.x, camPos.y, camPos.z) + reactphysics3d::Vector3(c.x, c.y, c.z) * 1000);
	SceneContactPoint* aimCollision = world->Raycast(ray, object);
	if (aimCollision->isHit) {
		Vector3 collisionVector = Vector3(aimCollision->hitPos) - Vector3(object->GetPhysicsObject()->getTransform().getPosition());

		float theta = atan2(collisionVector.z, collisionVector.x) * (180 / PI);
		Quaternion goatTargetRotation = Quaternion(Matrix4::Rotation(-theta - 90, Vector3(0, 1, 0)));
		goatStartRotation = Quaternion(object->GetPhysicsObject()->getTransform().getOrientation());
		Quaternion goatRealRotation = Quaternion::Lerp(goatStartRotation, goatTargetRotation, 0.5f);
		return goatRealRotation;
	}
	Matrix4 Yaw = cam->GetRotationYaw();
	return Quaternion(Yaw);
}

void TutorialGame::cameraInterpolation(Vector3 target, float dt) {
	Camera* currentCamera = world->GetMainCamera();
	Vector3 currentCamPos = currentCamera->GetPosition();
	Vector3 movement = target - currentCamPos;
	//float movementLength = movement.Length();

	currentCamera->SetPosition(currentCamPos + movement * cameraInterpBaseSpeed);
}

Vector3 TutorialGame::orbitCameraProcess(Vector3 objPos) {
	Quaternion rotationAmount = Quaternion(world->GetMainCamera()->GetRotationYaw()) * Quaternion(world->GetMainCamera()->GetRotationPitch());

	orbitScalar -= Window::GetMouse()->GetWheelMovement();
	orbitScalar = NCL::Maths::Clamp(orbitScalar, orbitScalarMin, orbitScalarMax);

	Vector3 camPos = objPos + rotationAmount * Vector3(0, 0, orbitScalar);
	Vector3 startPos = objPos + rotationAmount * Vector3(0, 0, 1.5);
	Vector3 direction = rotationAmount * Vector3(0, 0, 1);
	float rayLength = 100.0f;
	reactphysics3d::Ray ray = reactphysics3d::Ray(reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z), reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z) + reactphysics3d::Vector3(direction.x, direction.y, direction.z) * rayLength);
	SceneContactPoint* cameraCollision = world->Raycast(ray, lockedObject);
	if (cameraCollision->isHit) {
		float distance = cameraCollision->hitFraction * rayLength;
		if (distance < orbitScalar) {
			camPos = objPos + rotationAmount * Vector3(0, 0, distance);
		}
	}
	return camPos;
}

Vector3 TutorialGame::thirdPersonCameraProcess(Vector3 objPos) {

	objPos = objPos + Vector3(0, thirdPersonYScalar, 0);
	Quaternion rotationAmount = Quaternion(world->GetMainCamera()->GetRotationYaw()) * Quaternion(world->GetMainCamera()->GetRotationPitch());

	Vector3 endVector = Vector3(thirdPersonXScalar, 0, thirdPersonZScalar);

	Vector3 direction = rotationAmount * Vector3(thirdPersonXScalar, 0, 0);
	float rayLength = 100.0f;
	reactphysics3d::Ray ray = reactphysics3d::Ray(reactphysics3d::Vector3(objPos.x, objPos.y, objPos.z), reactphysics3d::Vector3(objPos.x, objPos.y, objPos.z) + reactphysics3d::Vector3(direction.x, direction.y, direction.z) * rayLength);
	SceneContactPoint* cameraCollision = world->Raycast(ray, lockedObject);
	if (cameraCollision->isHit) {
		float distance = cameraCollision->hitFraction * rayLength;
		if (distance < thirdPersonXScalar) {
			endVector.x = distance;
		}
	}

	Vector3 startPos = (objPos + rotationAmount * Vector3(endVector.x, 0, 0));
	Vector3 direction2 = rotationAmount * Vector3(0, 0, thirdPersonZScalar);
	reactphysics3d::Ray ray2 = reactphysics3d::Ray(reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z), reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z) + reactphysics3d::Vector3(direction2.x, direction2.y, direction2.z) * rayLength);
	SceneContactPoint* cameraCollision2 = world->Raycast(ray2, lockedObject);
	if (cameraCollision2->isHit) {
		float distance = cameraCollision2->hitFraction * rayLength;
		if (distance < thirdPersonZScalar) {
			endVector.z = distance;
		}
	}
	return objPos + rotationAmount * endVector;
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

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetPhysicsObject()->applyWorldForceAtCenterOfMass(lockedObject->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, -7));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(0, 2, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
		lockedObject->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(0, 50, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		lockedObject->GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(0, -2, 0));
	}
	
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetPhysicsObject()->applyWorldForceAtCenterOfMass(lockedObject->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, 7));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		lockedObject->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(0, -10, 0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	InitGameExamples();
	
	Vector3 startPos(70, 0, -10);
	
	TestPathfinding(startPos);
	testStateObject = AddStateObjectToWorld(reactphysics3d::Vector3(nodes[0].x, nodes[0].y, nodes[0].z), reactphysics3d::Quaternion::identity(), nodes);
	
	button = AddButtonToWorld(reactphysics3d::Vector3(0, -18, 0), reactphysics3d::Quaternion::identity());
	buildGameworld();
	//AddBreakableToWorld(Vector3( 200.0f,13.0f, 50.0f), 1.0f);
	//AddGWBlocksToWorld(Vector3(201.0f,  18.0f,  100.0f), Vector3(5, 5, 5));
	AddHedgeMazeToWorld();
	door = AddGWBlocksToWorld(reactphysics3d::Vector3(50,-13,50), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(5, 5, 5));
	door->GetRenderObject()->SetColour(Vector4(0,0,1,1));
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
				object = AddGWBlocksToWorld(reactphysics3d::Vector3(Nposition.x-200.0f, Nposition.y - 13.0f, Nposition.z - 50.0f), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(5, 5, 5));
				break;

			case 'b':
				object = AddBreakableToWorld(reactphysics3d::Vector3(Nposition.x - 200.0f, Nposition.y - 13.0f, Nposition.z - 50.0f), reactphysics3d::Quaternion::identity(), 1.0f);
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
				object = AddGWBlocksToWorld(reactphysics3d::Vector3(Nposition.x-45.0f , Nposition.y-13.0f , Nposition.z- 450.0f ), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(5, 5, 5));
				break;

			case 'b':
				object = AddBreakableToWorld(reactphysics3d::Vector3(Nposition.x - 45.0f, Nposition.y- 13.0f, Nposition.z - 450.0f), reactphysics3d::Quaternion::identity(), 1.0f);
				break;
			}
		}
	}
}
/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents) {
	GameObject* floor = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setType(reactphysics3d::BodyType::STATIC);
	body->setMass(0);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(halfextents);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	floor->SetPhysicsObject(body);
	floor->SetRenderObject(new RenderObject(body, Vector3(halfextents) * 2, cubeMesh, basicTex, basicShader));

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
Projectile* TutorialGame::AddSphereToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, float mass) {
	Projectile* sphere = new Projectile(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setMass(mass);
	reactphysics3d::SphereShape* shape = physics.createSphereShape(radius);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	reactphysics3d::Material material = collider->getMaterial();
	material.setBounciness(0.22f);
	
	collider->setMaterial(material);
	sphere->SetPhysicsObject(body);
	sphere->SetRenderObject(new RenderObject(body, Vector3(radius, radius, radius), sphereMesh, basicTex, basicShader));
	sphere->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	sphere->time = 1.5f;
	world->AddGameObject(sphere);

	return sphere;
}
GameObject* TutorialGame::AddBreakableToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, float mass) {
	GameObject* sphere = new GameObject(world);
	sphere->SetTag(2);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setMass(mass);
	reactphysics3d::SphereShape* shape = physics.createSphereShape(radius);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	reactphysics3d::Material material = collider->getMaterial();
	material.setBounciness(0.22f);
	collider->setMaterial(material);
	sphere->SetPhysicsObject(body);
	sphere->SetRenderObject(new RenderObject(body, Vector3(radius, radius, radius), sphereMesh, basicTex, basicShader));
	sphere->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents, float mass) {
	GameObject* cube = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setMass(mass);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(halfextents);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	cube->SetPhysicsObject(body);
	cube->SetRenderObject(new RenderObject(body, Vector3(halfextents) * 2, cubeMesh, basicTex, basicShader));

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddGWBlocksToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 halfextents) {
	GameObject* cube = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setType(reactphysics3d::BodyType::STATIC);
	body->setMass(0);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(halfextents);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	cube->SetPhysicsObject(body);
	cube->SetRenderObject(new RenderObject(body, Vector3(halfextents) * 2, cubeMesh, basicTex, basicShader));

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddButtonToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float mass) {
	GameObject* floor = new GameObject(world);
	floor->SetTag(3);
	reactphysics3d::Vector3 halfextents = reactphysics3d::Vector3(1.0f, 0.05f, 1.0f);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setType(reactphysics3d::BodyType::KINEMATIC);
	body->setMass(0);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(halfextents);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	floor->SetPhysicsObject(body);
	floor->SetRenderObject(new RenderObject(body, Vector3(halfextents) * 2, cubeMesh, basicTex, basicShader));
	floor->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddPlayerToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation) {
	GameObject* character = new GameObject(world);
	character->SetTag(1);
	character->setScore(0);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setAngularLockAxisFactor(reactphysics3d::Vector3(0, 1, 0));
	body->setMass(2.0f);
	reactphysics3d::SphereShape* shape = physics.createSphereShape(1.0f);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	character->SetPhysicsObject(body);
	character->SetRenderObject(new RenderObject(body, Vector3(1, 1, 1), charMesh, basicTex, charShader));
	character->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

	world->AddGameObject(character);
	character->SetWorldID(1);
	NetworkObject* n = new NetworkObject(*character, 1);

	return character;
}

GameObject* TutorialGame::AddPlayer2ToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation) {
	GameObject* character = new GameObject(world);
	character->SetTag(1);
	character->setScore(0);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setAngularLockAxisFactor(reactphysics3d::Vector3(0, 1, 0));
	body->setMass(2.0f);
	reactphysics3d::CapsuleShape* shape = physics.createCapsuleShape(1.5f, 2.5f);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	character->SetPhysicsObject(body);
	character->SetRenderObject(new RenderObject(body, Vector3(1, 1, 1), charMesh, nullptr, basicShader));
	character->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

	world->AddGameObject(character);
	character->SetWorldID(2);
	NetworkObject* n = new NetworkObject(*character, 2);

	return character;
}


GameObject* TutorialGame::AddEnemyToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation) {
	GameObject* character = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setAngularLockAxisFactor(reactphysics3d::Vector3(0, 1, 0));
	body->setMass(2.0f);
	reactphysics3d::CapsuleShape* shape = physics.createCapsuleShape(0.5f, 2.0f);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	character->SetPhysicsObject(body);
	character->SetRenderObject(new RenderObject(body, Vector3(1, 1, 1), enemyMesh, nullptr, basicShader));
	character->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEmitterToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation) {
	GameObject* emitter = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setType(reactphysics3d::BodyType::KINEMATIC);
	body->setMass(0.0f);
	reactphysics3d::SphereShape* shape = physics.createSphereShape(0.5f);

	std::vector <Vector3 > verts;

	for (int i = 0; i < 100; ++i) {
		float x = (float)(rand() % 100 - 50);
		float y = (float)(rand() % 100 - 50);
		float z = (float)(rand() % 100 - 50);

		verts.push_back(Vector3(x, y, z));

	}

	OGLMesh* pointSprites = new OGLMesh();
	pointSprites->SetVertexPositions(verts);
	pointSprites->SetPrimitiveType(GeometryPrimitive::Points);
	pointSprites->AddSubMesh(0, 100, 0);
	pointSprites->UploadToGPU();
	OGLShader* newShader = new OGLShader("scene.vert", "scene.frag", "pointGeom.glsl");
	Matrix4 modelMat = Matrix4::Translation(Vector3(0, 0, -30));

	emitter->SetPhysicsObject(body);
	emitter->SetRenderObject(new RenderObject(body, Vector3(1, 1, 1), pointSprites, OGLTexture::RGBATextureFromFilename("particle.tga"), newShader));
	world->AddGameObject(emitter);
	return emitter;
}

GameObject* TutorialGame::AddBonusToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation) {
	GameObject* apple = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setMass(1.0f);
	reactphysics3d::SphereShape* shape = physics.createSphereShape(0.5f);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	apple->SetPhysicsObject(body);
	apple->SetRenderObject(new RenderObject(body, Vector3(2, 2, 2), bonusMesh, nullptr, basicShader));

	world->AddGameObject(apple);

	return apple;
}
StateGameObject* TutorialGame::AddStateObjectToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, vector<Vector3> testNodes) {
	StateGameObject* apple = new StateGameObject(world,testNodes);
	apple->setTarget(player);
	apple->SetTag(5);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setMass(2.0f);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(reactphysics3d::Vector3(1.5f, 2.5f, 1.5f));
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	apple->SetPhysicsObject(body);
	apple->SetRenderObject(new RenderObject(body, Vector3(3, 5, 3), enemyMesh, nullptr, basicShader));

	world->AddGameObject(apple);

	return apple;
}
BTreeObject* TutorialGame::AddGooseToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, vector<Vector3> testNodes) {
	BTreeObject* apple = new BTreeObject(world,testNodes);
	apple->setTarget1(player);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setMass(2.0f);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(reactphysics3d::Vector3(0.5f, 0.5f, 0.5f));
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	apple->SetPhysicsObject(body);
	apple->SetRenderObject(new RenderObject(body, Vector3(2, 2, 2), gooseMesh, nullptr, basicShader));
	apple->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

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
	AddFloorToWorld(reactphysics3d::Vector3(0, -20, 0), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(200, 2, 150));
	AddFloorToWorld(reactphysics3d::Vector3( 0, -20, -400), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(100, 2, 75));
}

void TutorialGame::InitGameExamples() {
	player = AddPlayerToWorld(reactphysics3d::Vector3(-10, -15, -335), reactphysics3d::Quaternion::identity());

	AddRebWallMainToWorld(reactphysics3d::Vector3(-15, -18, -335), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 1, 1));

	AddEmitterToWorld(reactphysics3d::Vector3(-20, 5, -345), reactphysics3d::Quaternion::identity());
	LockCameraToObject(player);
	patrol = AddEnemyToWorld(reactphysics3d::Vector3(-20, 5, 20), reactphysics3d::Quaternion::identity());
	AddBonusToWorld(reactphysics3d::Vector3(10, 5, 0), reactphysics3d::Quaternion::identity());
	world->SetPlayer(player);

}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			AddSphereToWorld(reactphysics3d::Vector3(x * colSpacing, 10.0f, z * rowSpacing), reactphysics3d::Quaternion::identity(), radius, 1.0f);
		}
	}
	AddFloorToWorld(reactphysics3d::Vector3(0, -2, 0), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(200, 2, 150));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			reactphysics3d::Vector3 position = reactphysics3d::Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(0.5f, 0.5f, 0.5f));
			}
			else {
				AddSphereToWorld(position, reactphysics3d::Quaternion::identity(), 1.0f);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const reactphysics3d::Vector3& cubeHalfextents) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			AddCubeToWorld(reactphysics3d::Vector3(x * colSpacing, 10.0f, z * rowSpacing), reactphysics3d::Quaternion::identity(), cubeHalfextents, 1.0f);
		}
	}
}


// Rebellion Asset Objects
GameObject* TutorialGame::AddRebWallMainToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale) {
	GameObject* wall = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setType(reactphysics3d::BodyType::STATIC);
	body->setMass(0);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(reactphysics3d::Vector3(0.5 * scale.x, 3.5 * scale.y, 0.1 * scale.z));
	reactphysics3d::Transform collisionOffset(reactphysics3d::Vector3(0, 0, -1), reactphysics3d::Quaternion::identity());
	reactphysics3d::Collider* collider = body->addCollider(shape, collisionOffset);
	wall->SetPhysicsObject(body);
	wall->SetRenderObject(new RenderObject(body, Vector3(scale), corridorStraightMesh, basicTex, basicShader));

	world->AddGameObject(wall);

	return wall;
}


/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (inSelectionMode) {
		//Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray r = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
			Vector3 startPos = r.GetPosition();
			Vector3 endPos = r.GetPosition() + r.GetDirection() * 1000;
			reactphysics3d::Ray ray = reactphysics3d::Ray(reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z), reactphysics3d::Vector3(endPos.x, endPos.y, endPos.z));
			SceneContactPoint* closestCollision = world->Raycast(ray);
			Debug::DrawLine(startPos, endPos, Vector4(0, 1, 1, 1), 3);
			if (closestCollision->isHit) {
				selectionObject = (GameObject*)closestCollision->object;

				//selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				world->painted.push_back(closestCollision->hitPos);
				for (Vector4 x : world->painted) {
					std::cout << x<<"\n";
				}
				//std::cout << world->painted[0];
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
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
		Ray r = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		Vector3 startPos = r.GetPosition();
		Vector3 endPos = r.GetPosition() + r.GetDirection() * 1000;
		reactphysics3d::Ray ray = reactphysics3d::Ray(reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z), reactphysics3d::Vector3(endPos.x, endPos.y, endPos.z));
		SceneContactPoint* closestCollision = world->Raycast(ray);
		if (closestCollision->isHit) {
			if (closestCollision->object == selectionObject) {
				Vector3 force = r.GetDirection() * forceMagnitude;
				selectionObject->GetPhysicsObject()->applyWorldForceAtWorldPosition(reactphysics3d::Vector3(force.x, force.y, force.z), closestCollision->hitPos);
			}
		}
	}
}

void patrolMovement() {

 }

