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
#include "MapNode.h"
#include "windows.h"
#include "psapi.h"
#include <Xinput.h>
#include "Gamelock.h"
#include "Sound.h"

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
	mouseLock		= true;
	debug = false;
	world->SetPlayerHealth(1.0f);
	world->SetPlayerCoopHealth(1.0f);

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
	basicShader = renderer->LoadShader("scene.vert", "sceneAlternate.frag");
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

	InitSound();
	InitCamera();
	InitCameraSec();
	InitWorld();
	InitProjectiles();
	InitPaintOrb();

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

	delete[] projectiles;

	delete renderer;
	delete world;
	physics.destroyPhysicsWorld(physicsWorld);
}

void TutorialGame::UpdateGame(float dt) {
	//Debug::DrawAxisLines(Matrix4());
	accumulator += dt;

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::TAB)) {
		debug = !debug;
	}
	if (debug) {
		RenderDebug(dt);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::E) && freeCamera) {
		inSelectionMode = !inSelectionMode;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		freeCamera = !freeCamera;
		if (!freeCamera) inSelectionMode = false;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::O)) {
		mouseLock = !mouseLock;
	}

	if (freeCamera) {
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(mouseLock);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(mouseLock);
		}
		world->GetMainCamera()->UpdateCamera(dt);
		world->GetSecCamera()->UpdateCamera(dt);
	}
	else {
		Window::GetWindow()->ShowOSPointer(false);
		Window::GetWindow()->LockMouseToWindow(mouseLock);
		if (lockedObject == player && !GameLock::Player1lock) {//player movelock!
			MovePlayer(player, dt);
		}
	}

	world->SetPlayerHealth(health);
	world->SetPlayerCoopHealth(secHealth);
	if (GameLock::gamestart) {//gametime//////////////////////////////////////
		timeLimit -= dt;
		GameLock::gametime = timeLimit;
	}
	else {
		timeLimit = GameLock::gametime;
	}


	Debug::Print(std::to_string((int)timeLimit), Vector2(47, 4), Debug::WHITE);
	
	float scoreOne = world->getColourOneScore();
	Debug::Print(std::to_string((float)scoreOne), Vector2(80, 15), Debug::WHITE);
	float scoreTwo = world->getColourTwoScore();
	Debug::Print(std::to_string((float)scoreTwo), Vector2(80, 17), Debug::WHITE);

	UpdateKeys();

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
	PlayerPaintTracks(player,'r');
	PlayerPaintTracks(playerCoop, 'b');


	world->OperateOnContents([&](GameObject* o) {o->Update(dt); });
	world->UpdateWorld(dt);
	while (accumulator >= timeStep) {
		physicsWorld->update(timeStep);
		//std::cout << accumulator << "s in the accumulator \n";
		accumulator -= timeStep;
	}
	//std::cout << "<<<<<<<<<<frame \n";
	renderer->Update(dt);

	if (coopMode && !freeCamera && GameLock::gamemod == 2) {//player2 movelock!
		renderer->RenderSplitScreens();
		if (GameLock::gamestart && !GameLock::Player2lock)
			MovePlayerCoop(playerCoop, dt);
	}
	else
		renderer->Render();
		
	
	Debug::UpdateRenderables(dt);
}

void TutorialGame::RenderDebug(float dt) {
	std::string fps = "FPS: " + std::to_string((int)(1 / dt));
	Debug::Print(fps, Vector2(5, 8), Debug::WHITE);
	std::string ft = "Frame Time: " + std::to_string(1000.0f * dt) + "ms";
	Debug::Print(ft, Vector2(5, 13), Debug::WHITE);

	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

	std::string vramA = "Virtual Mem available: " + std::to_string(totalVirtualMem / (1024 * 1024)) + "MB";
	std::string vramT = "Virtual Mem in use: " + std::to_string(virtualMemUsedByMe / (1024 * 1024)) + "MB";
	Debug::Print(vramA, Vector2(5, 18), Debug::WHITE);
	Debug::Print(vramT, Vector2(5, 23), Debug::WHITE);

	std::string pramA = "Physical RAM available: " + std::to_string(totalPhysMem / (1024 * 1024)) + "MB";
	std::string pramT = "Physical RAM in use: " + std::to_string(physMemUsedByMe / (1024 * 1024)) + "MB";
	Debug::Print(pramA, Vector2(5, 28), Debug::WHITE);
	Debug::Print(pramT, Vector2(5, 33), Debug::WHITE);

	std::string paintAmount = "Paint Balls in World: " + std::to_string(world->GetPaintBalls());
	Debug::Print(paintAmount, Vector2(5, 38), Debug::WHITE);

	std::string nbRigidBodies = "Number of Rigid Bodies: " + std::to_string(physicsWorld->getNbRigidBodies());
	Debug::Print(nbRigidBodies, Vector2(5, 43), Debug::WHITE);
}

void TutorialGame::UpdateKeys()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physicsWorld->setIsGravityEnabled(useGravity);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::V) ) {
		
		initSplitScreen ? initSplitScreen = false : initSplitScreen = true;
	}

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
}

void TutorialGame::MovePlayer(PlayerObject* player, float dt) {
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
		camPos = orbitCameraProcess(objPos, *world->GetMainCamera(), lockedObject);
		cameraInterpolation(camPos, dt, *world->GetMainCamera());
	}
	else {
		camPos = thirdPersonCameraProcess(objPos, *world->GetMainCamera(), lockedObject);
		cameraInterpolation(camPos, dt, *world->GetMainCamera());
		camPos = world->GetMainCamera()->GetPosition();
	}

	Quaternion Yaw = Quaternion(world->GetMainCamera()->GetRotationYaw());
	player->SetYaw(reactphysics3d::Quaternion(Yaw.x, Yaw.y, Yaw.z, Yaw.w));

	Vector3 startVelocity = lockedObject->GetPhysicsObject()->getLinearVelocity();
	Vector3 endVelocity = Vector3(0, 0, 0);

	reactphysics3d::Ray ray = reactphysics3d::Ray(playerTransform.getPosition(), playerTransform.getPosition() + reactphysics3d::Vector3(0, -5, 0));
	SceneContactPoint* ground = world->Raycast(ray, player);
	player->setGrounded(ground->isHit);

	bool directionInput = false;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {

		Vector3 playermoveposition = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		MainScreenMoveMapping(playermoveposition, directionInput);///////////////////////////////////////////

		Vector3 trajectory = player->IsGrounded() ? Yaw * Vector3(0, 0, -25) : Yaw * Vector3(0, 0, -12);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(0, 0, -1);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {

		Vector3 playermoveposition = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		MainScreenMoveMapping(playermoveposition, directionInput);///////////////////////////////////////////

		Vector3 trajectory = player->IsGrounded() ? Yaw * Vector3(0, 0, 25) : Yaw * Vector3(0, 0, 12);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(0, 0, 1);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {

		Vector3 playermoveposition = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		MainScreenMoveMapping(playermoveposition, directionInput);///////////////////////////////////////////

		Vector3 trajectory = player->IsGrounded() ? Yaw * Vector3(-25, 0, 0) : Yaw * Vector3(-12, 0, 0);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(-1, 0, 0);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {

		Vector3 playermoveposition = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		MainScreenMoveMapping(playermoveposition, directionInput);///////////////////////////////////////////

		Vector3 trajectory = player->IsGrounded() ? Yaw * Vector3(25, 0, 0) : Yaw * Vector3(12, 0, 0);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(1, 0, 0);
		directionInput = true;
	}
	if (!directionInput && player->IsGrounded()) {
		float scalar = (0.95 - dt);
		player->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(startVelocity.x * scalar, startVelocity.y, startVelocity.z * scalar));
	}

	//if (directionInput && (endVelocity.Normalised() - Vector3(startVelocity).Normalised()).Length() > 1.25 && player->IsGrounded()) {
	if (directionInput && (endVelocity.Normalised() - Vector3(startVelocity).Normalised()).Length() > 1.25) {
		endVelocity.Normalise();
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(endVelocity.x, endVelocity.y, endVelocity.z) * 20);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && player->IsGrounded()) {
		Vector3 playerjumpos = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		MainScreenJumpMapping(playerjumpos);////////////////////////////////////////////////////

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


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F)) {
		Projectile* projectile = projectiles[currentProjectile];
		currentProjectile = (currentProjectile + 1) % 100;

		Quaternion Pitch = Quaternion(world->GetMainCamera()->GetRotationPitch());
		reactphysics3d::Quaternion reactPitch = reactphysics3d::Quaternion(Pitch.x, Pitch.y, Pitch.z, Pitch.w);

		Vector4 colourVector;
		switch (player->getPaintColour()) {
		case 'r':
			colourVector = Vector4(1, 0, 0, 1);
			break;
		case 'b':
			colourVector = Vector4(0, 0, 1, 1);
			break;
		}
		projectile->GetRenderObject()->SetColour(colourVector);
		projectile->setPaintColour(player->getPaintColour());
		projectile->Reset();
		projectile->GetPhysicsObject()->setTransform(reactphysics3d::Transform(player->GetPhysicsObject()->getTransform().getPosition() + player->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, -3), reactphysics3d::Quaternion(0, 0, 0, 1)));
		projectile->GetPhysicsObject()->setType(reactphysics3d::BodyType::DYNAMIC);
		projectile->GetPhysicsObject()->applyWorldForceAtCenterOfMass(player->GetPhysicsObject()->getTransform().getOrientation() * reactPitch * reactphysics3d::Vector3(0, 0, -500));
		/*sound mod is here! If you don't want to use it , just comment them out*/
		Vector3 sphereintipos = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		MainScreenFireMapping(sphereintipos);///////////////////////////////////////////////////////////////////////
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::Y)) {
		world->drawPaintNodes();
	}

	timedetection += dt;/////////////////////////////////////////

	// splines, curves, improve interpolation, TCB curves
}

void NCL::CSC8503::TutorialGame::MovePlayerCoop(PlayerObject* player, float dt)
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
		thirdPerson = !thirdPerson;
	}

	world->GetSecCamera()->ThirdPersonUpdateRot();

	reactphysics3d::Transform playerTransform = player->GetPhysicsObject()->getTransform();
	Vector3 objPos = Vector3(playerTransform.getPosition());

	Quaternion goatStartRotation;
	Quaternion goatRealRotation;

	Vector3 camPos;
	if (!thirdPerson) {
		camPos = orbitCameraProcess(objPos, *world->GetSecCamera(), lockedSecObject);
		cameraInterpolation(camPos, dt, *world->GetSecCamera());
	}
	else {
		camPos = thirdPersonCameraProcess(objPos, *world->GetSecCamera(), lockedSecObject);
		cameraInterpolation(camPos, dt, *world->GetSecCamera());
		camPos = world->GetSecCamera()->GetPosition();
	}

	Quaternion Yaw = Quaternion(world->GetSecCamera()->GetRotationYaw());
	player->SetYaw(reactphysics3d::Quaternion(Yaw.x, Yaw.y, Yaw.z, Yaw.w));

	Vector3 startVelocity = lockedSecObject->GetPhysicsObject()->getLinearVelocity();
	Vector3 endVelocity = Vector3(0, 0, 0);

	reactphysics3d::Ray ray = reactphysics3d::Ray(playerTransform.getPosition(), playerTransform.getPosition() + reactphysics3d::Vector3(0, -5, 0));
	SceneContactPoint* ground = world->Raycast(ray, player);
	player->setGrounded(ground->isHit);

	bool directionInput = false;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {

		Vector3 playermoveposition = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		SecScreenMoveMapping(playermoveposition, directionInput);///////////////////////////////////////////

		Vector3 trajectory = player->IsGrounded() ? Yaw * Vector3(0, 0, -25) : Yaw * Vector3(0, 0, -12);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(0, 0, -1);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {

		Vector3 playermoveposition = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		SecScreenMoveMapping(playermoveposition, directionInput);///////////////////////////////////////////

		Vector3 trajectory = player->IsGrounded() ? Yaw * Vector3(0, 0, 25) : Yaw * Vector3(0, 0, 12);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(0, 0, 1);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {

		Vector3 playermoveposition = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		SecScreenMoveMapping(playermoveposition, directionInput);///////////////////////////////////////////

		Vector3 trajectory = player->IsGrounded() ? Yaw * Vector3(-25, 0, 0) : Yaw * Vector3(-12, 0, 0);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(-1, 0, 0);
		directionInput = true;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {

		Vector3 playermoveposition = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		SecScreenMoveMapping(playermoveposition, directionInput);///////////////////////////////////////////

		Vector3 trajectory = player->IsGrounded() ? Yaw * Vector3(25, 0, 0) : Yaw * Vector3(12, 0, 0);
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(trajectory.x, trajectory.y, trajectory.z));
		endVelocity = endVelocity + Yaw * Vector3(1, 0, 0);
		directionInput = true;
	}
	if (!directionInput && player->IsGrounded()) {
		float scalar = (0.95 - dt);
		player->GetPhysicsObject()->setLinearVelocity(reactphysics3d::Vector3(startVelocity.x * scalar, startVelocity.y, startVelocity.z * scalar));
	}

	if (directionInput && (endVelocity.Normalised() - Vector3(startVelocity).Normalised()).Length() > 1.25 && player->IsGrounded()) {
		endVelocity.Normalise();
		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(endVelocity.x, endVelocity.y, endVelocity.z) * 20);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::J) && player->IsGrounded()) {

		Vector3 playerjumpos = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		SecScreenJumpMapping(playerjumpos);////////////////////////////////////////////////////

		player->GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(0, 1000, 0));
	}

	if (!thirdPerson) {
		Vector3 currentVelocity = Vector3(lockedSecObject->GetPhysicsObject()->getLinearVelocity());
		float theta = atan2(currentVelocity.z, currentVelocity.x) * (180 / PI);
		Quaternion goatTargetRotation = Quaternion(Matrix4::Rotation(-theta - 90, Vector3(0, 1, 0)));
		goatStartRotation = Quaternion(player->GetPhysicsObject()->getTransform().getOrientation());
		goatRealRotation = Quaternion::Lerp(goatStartRotation, goatTargetRotation, 0.5f);
	}
	else {
		Quaternion goatTargetRotation = thirdPersonRotationCalc(world, player, world->GetSecCamera(), camPos);
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

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::N)) {
		Projectile* projectile = projectiles[currentProjectile];
		currentProjectile = (currentProjectile + 1) % 100;

		Quaternion Pitch = Quaternion(world->GetMainCamera()->GetRotationPitch());
		reactphysics3d::Quaternion reactPitch = reactphysics3d::Quaternion(Pitch.x, Pitch.y, Pitch.z, Pitch.w);

		Vector4 colourVector;
		switch (player->getPaintColour()) {
		case 'r':
			colourVector = Vector4(1, 0, 0, 1);
			break;
		case 'b':
			colourVector = Vector4(0, 0, 1, 1);
			break;
		}
		projectile->GetRenderObject()->SetColour(colourVector);
		projectile->setPaintColour(player->getPaintColour());
		projectile->Reset();
		projectile->GetPhysicsObject()->setTransform(reactphysics3d::Transform(player->GetPhysicsObject()->getTransform().getPosition() + player->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, -3), reactphysics3d::Quaternion(0, 0, 0, 1)));
		projectile->GetPhysicsObject()->setType(reactphysics3d::BodyType::DYNAMIC);
		projectile->GetPhysicsObject()->applyWorldForceAtCenterOfMass(player->GetPhysicsObject()->getTransform().getOrientation() * reactPitch * reactphysics3d::Vector3(0, 0, -500));


		//char colourInput = player->getPaintColour();
		//Projectile* projectile = AddProjectileToWorld(player->GetPhysicsObject()->getTransform().getPosition() + player->GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, -3), reactphysics3d::Quaternion(0, 0, 0, 1), 0.3, colourInput);

		//Quaternion Pitch = Quaternion(world->GetSecCamera()->GetRotationPitch());
		//reactphysics3d::Quaternion reactPitch = reactphysics3d::Quaternion(Pitch.x, Pitch.y, Pitch.z, Pitch.w);

		//projectile->GetPhysicsObject()->applyWorldForceAtCenterOfMass(player->GetPhysicsObject()->getTransform().getOrientation() * reactPitch * reactphysics3d::Vector3(0, 0, -750));

		/*sound mod is here! If you don't want to use it , just comment them out*/
		Vector3 sphereintipos = player->GetPhysicsObject()->getTransform().getPosition();//////////////////////////////////////////////////
		SecScreenFireMapping(sphereintipos);///////////////////////////////////////////////////////////////////////
	}

	timedetection2 += dt;/////////////////////////////////////////

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

void TutorialGame::cameraInterpolation(Vector3 target, float dt, Camera& camera) {
	//Camera* currentCamera = world->GetMainCamera();
	Vector3 currentCamPos = camera.GetPosition();
	Vector3 movement = target - currentCamPos;
	//float movementLength = movement.Length();

	camera.SetPosition(currentCamPos + movement * cameraInterpBaseSpeed);
}

Vector3 TutorialGame::orbitCameraProcess(Vector3 objPos, Camera& camera, GameObject* ignorePlayer) {
	Quaternion rotationAmount = Quaternion(camera.GetRotationYaw()) * Quaternion(camera.GetRotationPitch());

	orbitScalar -= Window::GetMouse()->GetWheelMovement();
	orbitScalar = NCL::Maths::Clamp(orbitScalar, orbitScalarMin, orbitScalarMax);

	Vector3 camPos = objPos + rotationAmount * Vector3(0, 0, orbitScalar);
	Vector3 startPos = objPos + rotationAmount * Vector3(0, 0, 1.5);
	Vector3 direction = rotationAmount * Vector3(0, 0, 1);
	float rayLength = 100.0f;
	reactphysics3d::Ray ray = reactphysics3d::Ray(reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z), reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z) + reactphysics3d::Vector3(direction.x, direction.y, direction.z) * rayLength);
	SceneContactPoint* cameraCollision = world->Raycast(ray, ignorePlayer);
	if (cameraCollision->isHit) {
		float distance = cameraCollision->hitFraction * rayLength;
		if (distance < orbitScalar) {
			camPos = objPos + rotationAmount * Vector3(0, 0, distance);
		}
	}
	return camPos;
}

Vector3 TutorialGame::thirdPersonCameraProcess(Vector3 objPos, Camera& camera, GameObject* currPlayer) {

	objPos = objPos + Vector3(0, thirdPersonYScalar, 0);
	Quaternion rotationAmount = Quaternion(camera.GetRotationYaw()) * Quaternion(camera.GetRotationPitch());

	Vector3 endVector = Vector3(thirdPersonXScalar, 0, thirdPersonZScalar);

	Vector3 direction = rotationAmount * Vector3(thirdPersonXScalar, 0, 0);
	float rayLength = 100.0f;
	reactphysics3d::Ray ray = reactphysics3d::Ray(reactphysics3d::Vector3(objPos.x, objPos.y, objPos.z), reactphysics3d::Vector3(objPos.x, objPos.y, objPos.z) + reactphysics3d::Vector3(direction.x, direction.y, direction.z) * rayLength);
	SceneContactPoint* cameraCollision = world->Raycast(ray, currPlayer);
	if (cameraCollision->isHit) {
		float distance = cameraCollision->hitFraction * rayLength;
		if (distance < thirdPersonXScalar) {
			endVector.x = distance;
		}
	}

	Vector3 startPos = (objPos + rotationAmount * Vector3(endVector.x, 0, 0));
	Vector3 direction2 = rotationAmount * Vector3(0, 0, thirdPersonZScalar);
	reactphysics3d::Ray ray2 = reactphysics3d::Ray(reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z), reactphysics3d::Vector3(startPos.x, startPos.y, startPos.z) + reactphysics3d::Vector3(direction2.x, direction2.y, direction2.z) * rayLength);
	SceneContactPoint* cameraCollision2 = world->Raycast(ray2, currPlayer);
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
	//lockFirstObject = nullptr;
}

void TutorialGame::InitSound() {
	ISoundEngine* InitV = createIrrKlangDevice();
	Vector3 init = Vector3(20000, 0, 0);//voiceinit
	Init->ShootVoice(InitV, init);
	Init->HitVoice(InitV, init);
	Init->JumpVoice(InitV, init);
	Init->MoveVoice(InitV, init);
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
	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	InitGameExamples();
	
	Vector3 startPos(70, 0, -10);
	
	//TestPathfinding(startPos);
	//testStateObject = AddStateObjectToWorld(reactphysics3d::Vector3(nodes[0].x, nodes[0].y, nodes[0].z), reactphysics3d::Quaternion::identity(), nodes);
	
	//button = AddButtonToWorld(reactphysics3d::Vector3(0, -18, 0), reactphysics3d::Quaternion::identity());
	buildGameworld();
	
	InitDefaultFloor();
}

void TutorialGame::InitProjectiles() {
	for (int i = 0; i < 100; ++i) {
		projectiles[i] = AddProjectileToWorld(reactphysics3d::Vector3(0, -100, 0), reactphysics3d::Quaternion::identity(), 0.75, 1);
	}
}

void TutorialGame::InitPaintOrb() {
	GameObject* sphere = new GameObject(world);
	reactphysics3d::Transform transform(reactphysics3d::Vector3(0, -100, 0), reactphysics3d::Quaternion::identity());
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setMass(2);
	float radius = 1.5f;
	reactphysics3d::SphereShape* shape = physics.createSphereShape(radius);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	reactphysics3d::Material material = collider->getMaterial();
	material.setBounciness(0.22f);

	collider->setMaterial(material);
	//collider->setIsTrigger(true);
	sphere->SetPhysicsObject(body);
	sphere->SetRenderObject(new RenderObject(body, Vector3(radius, radius, radius), sphereMesh, nullptr, charShader));

	world->AddGameObject(sphere);

	world->setPaintOrb(sphere);

	sphere->GetPhysicsObject()->setType(reactphysics3d::BodyType::STATIC);
}

void TutorialGame::buildGameworld() {
	srand(time(0));
	int i = 0;
	worldGrid = new NavigationGrid("TestGrid5.txt");
	GridNode* nodes = worldGrid->GetAllNodes();
	int gridwidth = worldGrid->GetGridWidth();
	int gridheight = worldGrid->getGridHeight();
	for (int y = 0; y < gridheight; y++) {
		for (int x = 0; x < gridwidth; x++) {
			GridNode& n = nodes[(gridwidth * y) + x];
			Vector3 Nposition = n.position;
			GameObject* object;
			switch (n.type) {
			case 'N':
				break;
			case '.':
				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x-3.5, Nposition.y + 1, Nposition.z-3.5));
				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y + 1, Nposition.z - 3.5));
				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x+3.5, Nposition.y + 1, Nposition.z - 3.5));

				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x - 3.5, Nposition.y + 1, Nposition.z));
				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y + 1, Nposition.z));
				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x+3.5, Nposition.y + 1, Nposition.z));

				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x-3.5, Nposition.y + 1, Nposition.z+3.5));
				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y + 1, Nposition.z + 3.5));
				addPaintNodeToWorld(reactphysics3d::Vector3(Nposition.x + 3.5, Nposition.y + 1, Nposition.z + 3.5));
				break;
			case 'x':
				AddRebWallSquareToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break; 
			case 'n':
				AddRebWallNorthToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case 's':
				AddRebWallSouthToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case 'e':
				AddRebWallEastToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case 'w':
				AddRebWallWestToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case '=':
				AddRebWallDualHorizontalToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case 'h':
				AddRebWallDualVerticalToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case 'u':
				AddRebWallOpeningNorthToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case 'm':
				AddRebWallOpeningSouthToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case 'c':
				AddRebWallOpeningEastToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case 'd':
				AddRebWallOpeningWestToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case '9':
				AddRebWallCornerNorthEastToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case '7':
				AddRebWallCornerNorthWestToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case '3':
				AddRebWallCornerSouthEastToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
				break;
			case '1':
				AddRebWallCornerSouthWestToWorld(reactphysics3d::Vector3(Nposition.x, Nposition.y - 1, Nposition.z));
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
Projectile* TutorialGame::AddProjectileToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, float radius, char colour, float mass) {
	Projectile* sphere = new Projectile(world, 3.0f);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setMass(mass);
	body->setType(reactphysics3d::BodyType::STATIC);
	reactphysics3d::SphereShape* shape = physics.createSphereShape(radius);
	reactphysics3d::Collider* collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	reactphysics3d::Material material = collider->getMaterial();
	material.setBounciness(0.22f);
	
	collider->setMaterial(material);
	sphere->SetPhysicsObject(body);
	sphere->SetRenderObject(new RenderObject(body, Vector3(radius, radius, radius), sphereMesh, nullptr, charShader));

	Vector4 colourVector;
	switch (colour) {
	case 'r':
		colourVector = Vector4(1, 0, 0, 1);
		break;
	case 'b':
		colourVector = Vector4(0, 0, 1, 1);
		break;
	}
	sphere->setPaintColour(colour);

	sphere->GetRenderObject()->SetColour(colourVector);

	world->AddGameObject(sphere);
	world->AddPaintBall();

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

PlayerObject* TutorialGame::AddPlayerToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, int netID, int worldID) {
	PlayerObject* character = new PlayerObject(world);
	character->SetTag(1);
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
	character->SetWorldID(worldID);
	NetworkObject* n = new NetworkObject(*character, netID);

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

PlayerObject* NCL::CSC8503::TutorialGame::AddPlayerForCoop(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation)
{
	PlayerObject* character = new PlayerObject(world);
	character->SetTag(1);
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
	floor=AddFloorToWorld(reactphysics3d::Vector3(0, 0, 0), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(200, 2, 150));
}

void TutorialGame::InitGameExamples() {
	player = AddPlayerToWorld(reactphysics3d::Vector3(50, 2, 20), reactphysics3d::Quaternion::identity(), 1, 1);
	player->setPaintColour('r');
	playerCoop = AddPlayerForCoop(reactphysics3d::Vector3(40, 2, 20), reactphysics3d::Quaternion::identity());
	playerCoop->setPaintColour('b');

	//AddEmitterToWorld(reactphysics3d::Vector3(-20, 5, -345), reactphysics3d::Quaternion::identity());
	LockCameraToObject(player);
	LockCameraToObject2(playerCoop);

	patrol = AddEnemyToWorld(reactphysics3d::Vector3(-20, 5, 20), reactphysics3d::Quaternion::identity());
	//AddBonusToWorld(reactphysics3d::Vector3(10, 5, 0), reactphysics3d::Quaternion::identity());
	world->SetPlayer(player);
	world->SetPlayerCoop(playerCoop);
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
	GameObject* wall = new GameObject(world, "cube");
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setType(reactphysics3d::BodyType::STATIC);
	body->setMass(0);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(reactphysics3d::Vector3(0.5 * scale.x, 3.65 * scale.y, 0.1 * scale.z));
	reactphysics3d::Transform collisionOffset(reactphysics3d::Vector3(0, 0, -0.75 * scale.z), reactphysics3d::Quaternion::identity());
	reactphysics3d::Collider* collider = body->addCollider(shape, collisionOffset);
	wall->SetPhysicsObject(body);
	wall->SetRenderObject(new RenderObject(body, Vector3(scale), corridorStraightMesh, corridorTexture, basicShader));

	world->AddGameObject(wall);

	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 4.5, -0.75 * scale.z));
	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 7, -0.75 * scale.z));
	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 9.5, -0.75 * scale.z));

	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0.3 * scale.x, 4.5, -0.75 * scale.z));
	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0.3 * scale.x, 7, -0.75 * scale.z));
	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0.3 * scale.x, 9.5, -0.75 * scale.z));

	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(-0.3 * scale.x, 4.5, -0.75 * scale.z));
	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(-0.3 * scale.x, 7, -0.75 * scale.z));
	addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(-0.3 * scale.x, 9.5, -0.75 * scale.z));

	return wall;
}

GameObject* TutorialGame::AddRebWallRightToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale, bool nodes) {
	GameObject* wall = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setType(reactphysics3d::BodyType::STATIC);
	body->setMass(0);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(reactphysics3d::Vector3(0.5 * scale.x, 3.65 * scale.y, 0.1 * scale.z));
	reactphysics3d::Transform collisionOffset(reactphysics3d::Vector3(0, 0, -0.75 * scale.z), reactphysics3d::Quaternion::identity());
	reactphysics3d::Collider* collider = body->addCollider(shape, collisionOffset);
	wall->SetPhysicsObject(body);
	wall->SetRenderObject(new RenderObject(body, Vector3(scale), corridorCornerRightSideMesh, corridorTexture, basicShader));

	world->AddGameObject(wall);

	if (nodes) {
		addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 4.5, -0.75 * scale.z));
		addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 7, -0.75 * scale.z));
		addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 9.5, -0.75 * scale.z));
	}

	return wall;
}

GameObject* TutorialGame::AddRebWallLeftToWorld(const reactphysics3d::Vector3& position, const reactphysics3d::Quaternion& orientation, reactphysics3d::Vector3 scale, bool nodes) {
	GameObject* wall = new GameObject(world);
	reactphysics3d::Transform transform(position, orientation);
	reactphysics3d::RigidBody* body = physicsWorld->createRigidBody(transform);
	body->setType(reactphysics3d::BodyType::STATIC);
	body->setMass(0);
	reactphysics3d::BoxShape* shape = physics.createBoxShape(reactphysics3d::Vector3(0.5 * scale.x, 3.65 * scale.y, 0.1 * scale.z));
	reactphysics3d::Transform collisionOffset(reactphysics3d::Vector3(0, 0, -0.75 * scale.z), reactphysics3d::Quaternion::identity());
	reactphysics3d::Collider* collider = body->addCollider(shape, collisionOffset);
	wall->SetPhysicsObject(body);
	wall->SetRenderObject(new RenderObject(body, Vector3(scale), corridorCornerLeftSideMesh, corridorTexture, basicShader));

	world->AddGameObject(wall);

	if (nodes) {
		addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 4.5, -0.75 * scale.z));
		addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 7, -0.75 * scale.z));
		addPaintNodeToWorld(position + orientation * reactphysics3d::Vector3(0, 9.5, -0.75 * scale.z));
	}

	return wall;
}

void TutorialGame::AddRebWallSquareToWorld(const reactphysics3d::Vector3& position) {
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(-4.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 3, 1), true);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(8, 3, 1));
	AddRebWallRightToWorld(position + reactphysics3d::Vector3(4.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 3, 1), true);

	Quaternion objectRotation = Quaternion(Matrix4::Rotation(90, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallRightToWorld(position + reactphysics3d::Vector3(5.5, 0, -4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(5.5, 0, 0), realObjectRotation, reactphysics3d::Vector3(8, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(5.5, 0, 4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	
	objectRotation = Quaternion(Matrix4::Rotation(270, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);
	
	AddRebWallRightToWorld(position + reactphysics3d::Vector3(-5.5, 0, 4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-5.5, 0, 0), realObjectRotation, reactphysics3d::Vector3(8, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(-5.5, 0, -4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	
	objectRotation = Quaternion(Matrix4::Rotation(180, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);
	
	AddRebWallRightToWorld(position + reactphysics3d::Vector3(-4.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(8, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(4.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);
}

void TutorialGame::AddRebWallNorthToWorld(const reactphysics3d::Vector3& position) {
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(10, 3, 1));
}

void TutorialGame::AddRebWallSouthToWorld(const reactphysics3d::Vector3& position) {
	Quaternion objectRotation = Quaternion(Matrix4::Rotation(180, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
}

void TutorialGame::AddRebWallEastToWorld(const reactphysics3d::Vector3& position) {
	Quaternion objectRotation = Quaternion(Matrix4::Rotation(270, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-5.5, 0, 0), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
}

void TutorialGame::AddRebWallWestToWorld(const reactphysics3d::Vector3& position) {
	Quaternion objectRotation = Quaternion(Matrix4::Rotation(90, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(5.5, 0, 0), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
}

void TutorialGame::AddRebWallDualHorizontalToWorld(const reactphysics3d::Vector3& position) {
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(10, 3, 1));

	Quaternion objectRotation = Quaternion(Matrix4::Rotation(180, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
}

void TutorialGame::AddRebWallDualVerticalToWorld(const reactphysics3d::Vector3& position) {
	Quaternion objectRotation = Quaternion(Matrix4::Rotation(90, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(5.5, 0, 0), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));

	objectRotation = Quaternion(Matrix4::Rotation(270, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-5.5, 0, 0), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
}

void TutorialGame::AddRebWallOpeningSouthToWorld(const reactphysics3d::Vector3& position) {
	Quaternion objectRotation = Quaternion(Matrix4::Rotation(90, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallRightToWorld(position + reactphysics3d::Vector3(5.5, 0, -4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(5.5, 0, 0.5), realObjectRotation, reactphysics3d::Vector3(9, 3, 1));

	objectRotation = Quaternion(Matrix4::Rotation(270, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-5.5, 0, 0.5), realObjectRotation, reactphysics3d::Vector3(9, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(-5.5, 0, -4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);

	objectRotation = Quaternion(Matrix4::Rotation(180, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallRightToWorld(position + reactphysics3d::Vector3(-4.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(8, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(4.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
}

void TutorialGame::AddRebWallOpeningNorthToWorld(const reactphysics3d::Vector3& position) {
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(-4.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 3, 1), false);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(8, 3, 1));
	AddRebWallRightToWorld(position + reactphysics3d::Vector3(4.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 3, 1), false);
	
	Quaternion objectRotation = Quaternion(Matrix4::Rotation(90, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallMainToWorld(position + reactphysics3d::Vector3(5.5, 0, -0.5), realObjectRotation, reactphysics3d::Vector3(9, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(5.5, 0, 4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);

	objectRotation = Quaternion(Matrix4::Rotation(270, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallRightToWorld(position + reactphysics3d::Vector3(-5.5, 0, 4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-5.5, 0, -0.5), realObjectRotation, reactphysics3d::Vector3(9, 3, 1));
}

void TutorialGame::AddRebWallOpeningEastToWorld(const reactphysics3d::Vector3& position) {
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(-4.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 3, 1), true);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(9, 3, 1));

	Quaternion objectRotation = Quaternion(Matrix4::Rotation(270, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallRightToWorld(position + reactphysics3d::Vector3(-5.5, 0, 4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-5.5, 0, 0), realObjectRotation, reactphysics3d::Vector3(8, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(-5.5, 0, -4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);

	objectRotation = Quaternion(Matrix4::Rotation(180, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallRightToWorld(position + reactphysics3d::Vector3(-4.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(0.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(9, 3, 1));
}

void TutorialGame::AddRebWallOpeningWestToWorld(const reactphysics3d::Vector3& position) {
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-0.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(9, 3, 1));
	AddRebWallRightToWorld(position + reactphysics3d::Vector3(4.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 3, 1), true);

	Quaternion objectRotation = Quaternion(Matrix4::Rotation(90, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallRightToWorld(position + reactphysics3d::Vector3(5.5, 0, -4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(5.5, 0, 0), realObjectRotation, reactphysics3d::Vector3(8, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(5.5, 0, 4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);

	objectRotation = Quaternion(Matrix4::Rotation(180, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-0.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(9, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(4.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);
}

void TutorialGame::AddRebWallCornerNorthEastToWorld(const reactphysics3d::Vector3& position) {
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(-4.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 3, 1), true);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(1, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(10, 3, 1));

	Quaternion objectRotation = Quaternion(Matrix4::Rotation(270, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-5.5, 0, -1), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
	AddRebWallRightToWorld(position + reactphysics3d::Vector3(-5.5, 0, 4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
}

void TutorialGame::AddRebWallCornerNorthWestToWorld(const reactphysics3d::Vector3& position) {
	AddRebWallRightToWorld(position + reactphysics3d::Vector3(4.5, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(1, 3, 1), true);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-1, 0, 5.5), reactphysics3d::Quaternion::identity(), reactphysics3d::Vector3(10, 3, 1));

	Quaternion objectRotation = Quaternion(Matrix4::Rotation(90, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallMainToWorld(position + reactphysics3d::Vector3(5.5, 0, -1), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(5.5, 0, 4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
}

void TutorialGame::AddRebWallCornerSouthEastToWorld(const reactphysics3d::Vector3& position) {
	Quaternion objectRotation = Quaternion(Matrix4::Rotation(270, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-5.5, 0, 1), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(-5.5, 0, -4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);

	objectRotation = Quaternion(Matrix4::Rotation(180, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallRightToWorld(position + reactphysics3d::Vector3(-4.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(1, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
}

void TutorialGame::AddRebWallCornerSouthWestToWorld(const reactphysics3d::Vector3& position) {
	Quaternion objectRotation = Quaternion(Matrix4::Rotation(90, Vector3(0, 1, 0)));
	reactphysics3d::Quaternion realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallMainToWorld(position + reactphysics3d::Vector3(5.5, 0, 1), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
	AddRebWallRightToWorld(position + reactphysics3d::Vector3(5.5, 0, -4.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), true);

	objectRotation = Quaternion(Matrix4::Rotation(180, Vector3(0, 1, 0)));
	realObjectRotation = reactphysics3d::Quaternion(objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w);

	AddRebWallLeftToWorld(position + reactphysics3d::Vector3(4.5, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(1, 3, 1), false);
	AddRebWallMainToWorld(position + reactphysics3d::Vector3(-1, 0, -5.5), realObjectRotation, reactphysics3d::Vector3(10, 3, 1));
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

		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT) || Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
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

				int colourInt = (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::LEFT)) ? 1 : 2;
				Vector4 paintCollision = Vector4(closestCollision->hitPos, colourInt);

				world->painted.push_back(paintCollision);
				for (Vector4 x : world->painted) {
					//std::cout << x<<"\n";
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

void TutorialGame::PlayerPaintTracks(PlayerObject* player, char paintColour) {
	if (player->IsGrounded()) {
		//std::cout << player->collisionPoint << std::endl;
		float distance = sqrt(pow(player->collisionPoint.x - player->currentPos.x, 2) + pow(player->collisionPoint.z - player->currentPos.z, 2));
		if (distance > 1) {
			floor->GetRenderObject()->PaintSpray(player->collisionPoint,paintColour);
			player->currentPos = player->collisionPoint;

			world->testPaintNodes(player->collisionPoint, paintColour);
		}
	}
}

// Paint Node Functions
void TutorialGame::addPaintNodeToWorld(Vector3 location) {
	PaintNode* node = new PaintNode(location);
	world->AddPaintNode(node);
}

void TutorialGame::addMapNodeToWorld(Vector3 location) {
	MapNode* node = new MapNode(location);
	world->AddMapNode(node);
}

//SOUND MOD Functions
void TutorialGame::MainScreenFireMapping(Vector3 sphereintipos) {
	if (initSplitScreen) {
		Vector3 fireposition = sphereintipos - world->GetMainCamera()->GetPosition();
		Vector3 fireposition2 = sphereintipos - world->GetSecCamera()->GetPosition();
		firevoice->fireSoundMapping(fireposition, fireposition2);
	}
	else {
		Vector3 fireposition = sphereintipos - world->GetMainCamera()->GetPosition();
		ISoundEngine* fire = createIrrKlangDevice();
		firevoice->ShootVoice(fire, fireposition);
	}

}

void TutorialGame::MainScreenMoveMapping(Vector3 playermoveposition, bool directionInput) {
	if ((timedetection == 0 || timedetection >= 1.0f) && player->IsGrounded() && directionInput == false) {
		timedetection = 0;
		Vector3 playerrunposition = player->GetPhysicsObject()->getTransform().getPosition();
		if (initSplitScreen) {
			Vector3 runposition = playerrunposition - world->GetMainCamera()->GetPosition();
			Vector3 runposition2 = playerrunposition - world->GetSecCamera()->GetPosition();
			ISoundEngine* run = createIrrKlangDevice();
			movevoice->moveSoundMapping(runposition, runposition2);
		}
		else {
			Vector3 runposition = playerrunposition - world->GetMainCamera()->GetPosition();
			ISoundEngine* run = createIrrKlangDevice();
			movevoice->MoveVoice(run, runposition);
		}
	}
}

void TutorialGame::MainScreenJumpMapping(Vector3 playerjumpos) {
	if (initSplitScreen) {
		Vector3 jumpposition = playerjumpos - world->GetMainCamera()->GetPosition();
		Vector3 jumpposition2 = playerjumpos - world->GetSecCamera()->GetPosition();
		jumpvoice->jumpSoundMapping(jumpposition, jumpposition2);
	}
	else {
		Vector3 jumpposition = playerjumpos - world->GetMainCamera()->GetPosition();
		ISoundEngine* run = createIrrKlangDevice();
		jumpvoice->JumpVoice(run, jumpposition);
	}
}

void TutorialGame::SecScreenFireMapping(Vector3 sphereintipos) {
	if (initSplitScreen) {
		Vector3 fireposition = sphereintipos - world->GetMainCamera()->GetPosition();
		Vector3 fireposition2 = sphereintipos - world->GetSecCamera()->GetPosition();
		firevoice->fireSoundMapping(fireposition2, fireposition);
	}
	else {
		Vector3 fireposition = sphereintipos - world->GetMainCamera()->GetPosition();
		ISoundEngine* fire = createIrrKlangDevice();
		firevoice->ShootVoice(fire, fireposition);
	}

}

void TutorialGame::SecScreenMoveMapping(Vector3 playermoveposition, bool directionInput) {
	if ((timedetection2 == 0 || timedetection2 >= 2.0f) && player->IsGrounded() && directionInput == false) {
		timedetection2 = 0;
		Vector3 playerrunposition = player->GetPhysicsObject()->getTransform().getPosition();
		if (initSplitScreen) {
			Vector3 runposition = playerrunposition - world->GetMainCamera()->GetPosition();
			Vector3 runposition2 = playerrunposition - world->GetSecCamera()->GetPosition();
			ISoundEngine* run = createIrrKlangDevice();
			movevoice->moveSoundMapping(runposition2, runposition);
		}
		else {
			Vector3 runposition = playerrunposition - world->GetMainCamera()->GetPosition();
			ISoundEngine* run = createIrrKlangDevice();
			movevoice->MoveVoice(run, runposition);
		}
	}
}

void TutorialGame::SecScreenJumpMapping(Vector3 playerjumpos) {
	if (initSplitScreen) {
		Vector3 jumpposition = playerjumpos - world->GetMainCamera()->GetPosition();
		Vector3 jumpposition2 = playerjumpos - world->GetSecCamera()->GetPosition();
		jumpvoice->jumpSoundMapping(jumpposition2, jumpposition);
	}
	else {
		Vector3 jumpposition = playerjumpos - world->GetMainCamera()->GetPosition();
		ISoundEngine* run = createIrrKlangDevice();
		jumpvoice->JumpVoice(run, jumpposition);
	}
}